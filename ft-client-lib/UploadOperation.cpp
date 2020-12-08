// Copyright (C) 2009,2010,2011,2012 GlavSoft LLC.
// All rights reserved.
//
//-------------------------------------------------------------------------
// This file is part of the TightVNC software.  Please visit our Web site:
//
//                       http://www.tightvnc.com/
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//-------------------------------------------------------------------------
//

#include "UploadOperation.h"

#include "ft-common/WinFilePath.h"
#include "ft-common/FolderListener.h"
#include "file-lib/EOFException.h"

UploadOperation::UploadOperation(LogWriter *logWriter,
                                 FileInfo fileToUpload,
                                 const TCHAR *pathToSourceRoot,
                                 const TCHAR *pathToTargetRoot)
: CopyOperation(logWriter),
  m_file(0), m_fis(0), m_gotoChild(false), m_gotoParent(false), m_firstUpload(true),
  m_remoteFilesInfo(0), m_remoteFilesCount(0)
{
  m_pathToSourceRoot.setString(pathToSourceRoot);
  m_pathToTargetRoot.setString(pathToTargetRoot);

  m_toCopy = new FileInfoList(fileToUpload);

  changeFileToUpload(m_toCopy);
}

UploadOperation::UploadOperation(LogWriter *logWriter,
                                 const FileInfo *filesToUpload, size_t filesCount,
                                 const TCHAR *pathToSourceRoot,
                                 const TCHAR *pathToTargetRoot)
: CopyOperation(logWriter),
  m_file(0), m_fis(0), m_gotoChild(false), m_gotoParent(false), m_firstUpload(true),
  m_remoteFilesInfo(0), m_remoteFilesCount(0)
{
  m_pathToSourceRoot.setString(pathToSourceRoot);
  m_pathToTargetRoot.setString(pathToTargetRoot);

  m_toCopy = new FileInfoList(filesToUpload, filesCount);

  changeFileToUpload(m_toCopy);
}

UploadOperation::~UploadOperation()
{
  if (m_toCopy != NULL) {
    delete m_toCopy->getRoot();
  }
  if (m_fis != NULL) {
    try { m_fis->close(); } catch (IOException) { }
    delete m_fis;
  }
  if (m_file != NULL) {
    delete m_file;
  }
  releaseRemoteFilesInfo();
}

void UploadOperation::start()
{
  //
  // Reset flags
  //

  m_gotoChild = false;
  m_gotoParent = false;

  // Notify listeners that operation have started
  notifyStart();

  // Get total size of files to upload and null bytes processed count
  m_totalBytesToCopy = getInputFilesSize();
  m_totalBytesCopied = 0;

  //
  // Send file list request to know filelist of remote destination directory.
  // Real file upload will start when reply for this request will be received
  //

  m_firstUpload = true;

  m_sender->sendFileListRequest(m_pathToTargetRoot.getString(),
                                m_replyBuffer->isCompressionSupported());
}

void UploadOperation::onUploadReply(DataInputStream *input)
{
  sendFileDataChunk();
}

void UploadOperation::onUploadDataReply(DataInputStream *input)
{
  if (isTerminating()) {
    gotoNext();
    return ;
  }

  sendFileDataChunk();
}

void UploadOperation::onUploadEndReply(DataInputStream *input)
{
  // Cleanup
  try { m_fis->close(); } catch (...) { }
  delete m_fis;
  m_fis = NULL;

  delete m_file;
  m_file = NULL;

  // Upload next file in the list
  gotoNext();
}

void UploadOperation::onMkdirReply(DataInputStream *input)
{
  // Upload next file in the list
  gotoNext();
}

void UploadOperation::onLastRequestFailedReply(DataInputStream *input)
{
  StringStorage errDesc;

  m_replyBuffer->getLastErrorMessage(&errDesc);

  notifyFailedToUpload(errDesc.getString());

  //
  // If this LRF message comes to file list request, then
  // don't need to upload next file, we must execute "special message handler".
  //

  if (specialHandler()) {
    return ;
  }

  gotoNext();
}

void UploadOperation::onFileListReply(DataInputStream *input)
{
  initRemoteFiles(m_replyBuffer->getFilesInfo(),
                  m_replyBuffer->getFilesInfoCount());
  specialHandler();
}

void UploadOperation::killOp()
{
  //
  // If not files to upload, than clear memory and
  // operation is finished
  //

  delete m_toCopy->getRoot();
  m_toCopy = NULL;

  // Notify listeners that operation ended
  notifyFinish();
}

bool UploadOperation::specialHandler()
{
  // If first upload that start real upload operation
  if (m_firstUpload) {
    m_firstUpload = false;
    startUpload();
    return true;
  }
  // Call real gotoNext method
  if (m_gotoChild) {
    m_gotoChild = false;
    gotoNext(false);
    return true;
  }
  if (m_gotoParent) {
    m_gotoParent = false;
    gotoNext(false);
    return true;
  }
  return false;
}

UINT64 UploadOperation::getInputFilesSize()
{
  UINT64 totalFilesSize = 0;

  FileInfoList *fil = m_toCopy;
  while (fil != NULL) {
    StringStorage pathNoRoot;
    StringStorage pathToFile;

    fil->getAbsolutePath(&pathNoRoot, _T('\\'));

    pathToFile.setString(m_pathToSourceRoot.getString());
    pathToFile.appendString(_T("\\"));
    pathToFile.appendString(pathNoRoot.getString());

    totalFilesSize += getFileSize(pathToFile.getString());

    fil = fil->getNext();
  }

  return totalFilesSize;
}

UINT64 UploadOperation::getFileSize(const TCHAR *pathToFile)
{
  UINT64 fileSize = 0;

  File file(pathToFile);

  if (file.isDirectory()) {
    UINT32 filesCount = 0;
    file.list(NULL, &filesCount);

    StringStorage *fileNames = new StringStorage[filesCount];
    file.list(fileNames, NULL);

    for (UINT32 i = 0; i < filesCount; i++) {
      File subfile(pathToFile, fileNames[i].getString());

      StringStorage pathToSubFile;
      subfile.getPath(&pathToSubFile);

      fileSize += getFileSize(pathToSubFile.getString());
    }

    delete[] fileNames;
  } else {
    INT64 len = file.length();
    if (len == -1) {
      fileSize = 0;
    } else {
      fileSize = (UINT64)len;
    }
  }

  return fileSize;
}

void UploadOperation::startUpload()
{
  if (isTerminating()) {
    killOp();
    return ;
  } // terminate operation is needed

  // Current file info
  FileInfo *fileInfo = m_toCopy->getFileInfo();

  // Logging
  if (m_toCopy->getFirst()->getParent() == NULL) {
    StringStorage message;

    message.format(_T("Uploading '%s' %s"), m_pathToSourceFile.getString(),
                   fileInfo->isDirectory() ? _T("folder") : _T("file"));

    notifyInformation(message.getString());
  } // logging

  if (fileInfo->isDirectory()) {
    processFolder();
  } else {
    processFile();
  } // if not directory

  if (isTerminating()) {
    killOp();
    return ;
  } // terminate operation is needed
} // void

void UploadOperation::processFolder()
{
  StringStorage message;

  // Try list files from folder
  FolderListener listener(m_pathToSourceFile.getString());
  if (listener.list()) {
    m_toCopy->setChild(listener.getFilesInfo(), listener.getFilesCount());
  } else {
    // Logging
    StringStorage message;

    message.format(_T("Error: failed to get file list in local folder '%s'"),
                   m_pathToSourceFile.getString());

    notifyError(message.getString());
  }

  // Send request to create folder
  m_sender->sendMkDirRequest(m_pathToTargetFile.getString());
}

void UploadOperation::processFile()
{
  //
  // Cleanup
  //

  if (m_fis != NULL) {
    try { m_fis->close(); } catch (...) { }
    delete m_fis;
  }
  if (m_file != NULL) {
    delete m_file;
  }

  UINT64 initialFileOffset = 0;

  // Search if file already exists on remote machine
  for (UINT32 i = 0; i < m_remoteFilesCount; i++) {
    FileInfo *localFileInfo = m_toCopy->getFileInfo();
    FileInfo *remoteFileInfo = &m_remoteFilesInfo[i];

    const TCHAR *remoteFileName = remoteFileInfo->getFileName();
    const TCHAR *localFileName = localFileInfo->getFileName();

    // File collision, show file exist dialog
    if (_tcscmp(localFileName, remoteFileName) == 0) {

      //
      // Copy listener must decide what to do with this situation
      //

      int action = m_copyListener->targetFileExists(localFileInfo,
                                                    remoteFileInfo,
                                                    m_pathToTargetFile.getString());

      switch (action) {
      case CopyFileEventListener::TFE_OVERWRITE:
        break;
      case CopyFileEventListener::TFE_APPEND:
        initialFileOffset = remoteFileInfo->getSize();
        m_totalBytesCopied += initialFileOffset;
        break;
      case CopyFileEventListener::TFE_SKIP:
        m_totalBytesCopied += localFileInfo->getSize();
        gotoNext();
        return ;
      case CopyFileEventListener::TFE_CANCEL:
        if (!isTerminating()) {
          terminate();
        } // if not terminating
        return ;
      default: _ASSERT(FALSE);
      } // switch (action)
    } // if file already exists
  } // for all files in remote files

  // Trying to open file for reading
  m_file = new File(m_pathToSourceFile.getString());
  try {
    StringStorage path;
    m_file->getPath(&path);
    m_fis = new WinFileChannel(path.getString(), F_READ, FM_OPEN);
    // Try to seek
    m_fis->seek((INT64)initialFileOffset);

  } catch (Exception &ioEx) {
    notifyFailedToUpload(ioEx.getMessage());
    gotoNext();
    return ;
  } // try / catch

  bool overwrite = (initialFileOffset == 0);

  m_sender->sendUploadRequest(m_pathToTargetFile.getString(), overwrite,
                              initialFileOffset);
} // void

void UploadOperation::sendFileDataChunk()
{
  _ASSERT(m_fis != NULL);

  const size_t bufferSize = 1024 * 8;
  vector<char> buffer(bufferSize);
  UINT32 read = 0;
  try {
    size_t portion = m_fis->read(&buffer.front(), bufferSize);
    _ASSERT((UINT32)portion == portion);
    read = (UINT32)portion;
  } catch (EOFException) {

    //
    // End of file.
    //

    m_fis->close();

    UINT64 lastModified = 0;

    try {
      lastModified = m_file->lastModified();
    } catch (IOException) { } // try / catch

    m_sender->sendUploadEndRequest(0, lastModified);
    return ;

  } catch (IOException &ioEx) {
    notifyFailedToUpload(ioEx.getMessage());
    gotoNext();
    return ;
  } // try / catch

  try {
    m_sender->sendUploadDataRequest(&buffer.front(), read, false);
    m_totalBytesCopied += read;

    // Notify listener, that data chunk is copied
    if (m_copyListener != NULL) {
      m_copyListener->dataChunkCopied(m_totalBytesCopied,
                                      m_totalBytesToCopy);
    }
  } catch (IOException &ioEx) {
    throw ioEx;
  } // try / catch
}

void UploadOperation::gotoNext()
{
  gotoNext(true);
}

void UploadOperation::gotoNext(bool fake)
{
  FileInfoList *current = m_toCopy;

  bool hasChild = current->getChild() != NULL;
  bool hasNext = current->getNext() != NULL;
  bool hasParent = current->getFirst()->getParent() != NULL;

  if (hasChild) {
    if (fake) {
      m_gotoChild = true;

      m_sender->sendFileListRequest(m_pathToTargetFile.getString(),
                                    m_replyBuffer->isCompressionSupported());
      return ;
    } else {
      // If it has child, we must upload child file list first
      changeFileToUpload(current->getChild());
      startUpload();
    }
  } else if (hasNext) {
    // If it has no child, but has next file, we must upload next file
    changeFileToUpload(current->getNext());
    startUpload();
  } else {

    //
    // If it has no child and not next, but has parent file,
    // we must go to parent file (folder i mean), set childs to NULL
    // cause we already upload child files and go to next file.
    //

    FileInfoList *first = current->getFirst();
    if (hasParent) {
      if (fake) {
        m_gotoParent = true;

        StringStorage pathToRemoteFolder(m_pathToTargetRoot.getString());

        FileInfoList *parentOfCurrent = m_toCopy->getFirst()->getParent();
        FileInfoList *parentOfParent = NULL;

        if (parentOfCurrent != NULL) {
          parentOfParent = parentOfCurrent->getFirst()->getParent();
        }

        if (parentOfParent != NULL) {
          getRemotePath(parentOfParent, m_pathToTargetRoot.getString(), &pathToRemoteFolder);
        }

        m_sender->sendFileListRequest(pathToRemoteFolder.getString(),
                                      m_replyBuffer->isCompressionSupported());
        return ;
      } else {
        changeFileToUpload(first->getParent());
        m_toCopy->setChild(NULL, 0);
        gotoNext();
      }
    } else {
      killOp();
    } // if / else
  } // if / else
} // void

void UploadOperation::releaseRemoteFilesInfo()
{
  if (m_remoteFilesInfo != NULL) {
    delete[] m_remoteFilesInfo;
    m_remoteFilesInfo = NULL;
  }
  m_remoteFilesCount = 0;
}

void UploadOperation::initRemoteFiles(FileInfo *remoteFilesInfo, UINT32 count)
{
  releaseRemoteFilesInfo();

  m_remoteFilesCount = count;
  m_remoteFilesInfo = new FileInfo[count];
  for (UINT32 i = 0; i < count; i++) {
    m_remoteFilesInfo[i] = remoteFilesInfo[i];
  }
}

void UploadOperation::notifyFailedToUpload(const TCHAR *errorDescription)
{
  StringStorage message;

  message.format(_T("Error: failed to upload '%s' %s (%s)"),
                 m_pathToSourceFile.getString(),
                 m_toCopy->getFileInfo()->isDirectory() ? _T("folder") : _T("file"),
                 errorDescription);

  notifyError(message.getString());
}

void UploadOperation::changeFileToUpload(FileInfoList *toUpload)
{
  m_toCopy = toUpload;

  getLocalPath(m_toCopy, m_pathToSourceRoot.getString(), &m_pathToSourceFile);
  getRemotePath(m_toCopy, m_pathToTargetRoot.getString(), &m_pathToTargetFile);
}
