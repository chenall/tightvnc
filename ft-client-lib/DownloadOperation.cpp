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

#include "DownloadOperation.h"

DownloadOperation::DownloadOperation(LogWriter *logWriter,
                                     const FileInfo *filesToDownload,
                                     size_t filesCount,
                                     const TCHAR *pathToTargetRoot,
                                     const TCHAR *pathToSourceRoot)
: CopyOperation(logWriter),
  m_file(0),
  m_fos(0),
  m_fileOffset(0)
{
  m_pathToSourceRoot.setString(pathToSourceRoot);
  m_pathToTargetRoot.setString(pathToTargetRoot);

  m_toCopy = new FileInfoList(filesToDownload, filesCount);

  changeFileToDownload(m_toCopy);
}

DownloadOperation::~DownloadOperation()
{
  if (m_toCopy != NULL) {
    delete m_toCopy->getRoot();
  }
  if (m_fos != NULL) {
    try { m_fos->close(); } catch (IOException) { }
    delete m_fos;
  }
  if (m_file != NULL) {
    delete m_file;
  }
}

void DownloadOperation::start()
{
  m_foldersToCalcSizeLeft = 0;

  m_totalBytesToCopy = 0;
  m_totalBytesCopied = 0;

  // Notify listeners that operation have started
  notifyStart();

  //
  // Try to calculate input files size.
  //
  // Then this state will be finished, we can
  // start files download.
  //
  // See decFoldersToCalcSizeCount, onDirSizeReply, onLastRequestFailed
  // methods.
  //

  tryCalcInputFilesSize();

  if (m_foldersToCalcSizeLeft == 0) {
    startDownload();
  }
}

void DownloadOperation::onFileListReply(DataInputStream *input)
{
  m_toCopy->setChild(m_replyBuffer->getFilesInfo(),
                     m_replyBuffer->getFilesInfoCount());
  gotoNext();
}

void DownloadOperation::onDownloadReply(DataInputStream *input)
{
  //
  // Cleanup
  //

  if (m_fos != NULL) {
    try { m_fos->close(); } catch (...) { }
    delete m_fos;
  }
  if (m_file != NULL) {
    delete m_file;
  }

  //
  // Try to create file on local file system and open it for writting
  //

  m_file = new File(m_pathToTargetFile.getString());

  if (m_fileOffset == 0) {
    m_file->truncate();
  }

  try {
    StringStorage path;
    m_file->getPath(&path);
    m_fos = new WinFileChannel(path.getString(), F_WRITE, FM_OPEN);
    // Seek to initial file position to continue writting
    m_fos->seek((INT64)m_fileOffset);
    m_totalBytesCopied += m_fileOffset;
  } catch (Exception &ioEx) {
    notifyFailedToDownload(ioEx.getMessage());
    gotoNext();
    return;
  }

  //
  // Send first request for file data
  //

  UINT32 dataSize = 1024 * 8;
  bool compression = m_replyBuffer->isCompressionSupported();

  m_sender->sendDownloadDataRequest(dataSize,
                                    compression);
}

void DownloadOperation::onDownloadDataReply(DataInputStream *input)
{
  if (isTerminating()) {
    gotoNext();
    return ;
  }

  UINT32 bufferSize = 1024 * 8;

  try {
    DataOutputStream dos(m_fos);
    if (!m_replyBuffer->getDownloadBuffer().empty()) {
      dos.writeFully(&m_replyBuffer->getDownloadBuffer().front(),
                     m_replyBuffer->getDownloadBufferSize());
    }
  } catch (IOException &ioEx) {
    notifyFailedToDownload(ioEx.getMessage());
    gotoNext();
    return ;
  }

  //
  // Notify that we receive some data
  //

  m_totalBytesCopied += m_replyBuffer->getDownloadBufferSize();

  if (m_copyListener != NULL) {
    m_copyListener->dataChunkCopied(m_totalBytesCopied, m_totalBytesToCopy);
  }

  //
  // Send next download data request
  //

  bool compression = m_replyBuffer->isCompressionSupported();

  m_sender->sendDownloadDataRequest(bufferSize, compression);
}

void DownloadOperation::onDownloadEndReply(DataInputStream *input)
{
  //
  // Cleanup
  //

  try { m_fos->close(); } catch (...) { }
  delete m_fos;
  m_fos = NULL;

  try {
    m_file->setLastModified(m_replyBuffer->getDownloadLastModified());
  } catch (...) {
    notifyFailedToDownload(_T("Cannot set modification time"));
  }

  delete m_file;
  m_file = NULL;

  gotoNext();
}

void DownloadOperation::onLastRequestFailedReply(DataInputStream *input)
{
  //
  // This LRF message received from get folder size request
  // we do need to download next file
  //

  if (m_foldersToCalcSizeLeft > 0) {
    decFoldersToCalcSizeCount();
  } else {
    // Logging
    StringStorage message;

    m_replyBuffer->getLastErrorMessage(&message);

    notifyFailedToDownload(message.getString());

    // Download next file
    gotoNext();
  }
}

void DownloadOperation::onDirSizeReply(DataInputStream *input)
{
  m_totalBytesToCopy += m_replyBuffer->getDirSize();
  decFoldersToCalcSizeCount();
}

void DownloadOperation::startDownload()
{
  if (isTerminating()) {
    killOp();
    return ;
  } // if terminating

  FileInfo *fileInfo = m_toCopy->getFileInfo();

  // Logging
  if (m_toCopy->getFirst()->getParent() == NULL) {
    StringStorage message;

    message.format(_T("Downloading '%s' %s"), m_pathToTargetFile.getString(),
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
  } // if terminating
}

void DownloadOperation::processFile()
{
  m_fileOffset = 0;

  File targetFile(m_pathToTargetFile.getString());

  if (targetFile.exists()) {
    FileInfo *sourceFileInfo = m_toCopy->getFileInfo();
    FileInfo targetFileInfo(&targetFile);

    //
    // Copy listener must decide what to do with this situation
    //

    int action = m_copyListener->targetFileExists(sourceFileInfo,
                                                  &targetFileInfo,
                                                  m_pathToTargetFile.getString());
    switch (action) {
    case CopyFileEventListener::TFE_OVERWRITE:
      break;
    case CopyFileEventListener::TFE_SKIP:
      m_totalBytesCopied += sourceFileInfo->getSize();
      gotoNext();
      return ;
    case CopyFileEventListener::TFE_APPEND:
      m_fileOffset = targetFileInfo.getSize();
      break;
    case CopyFileEventListener::TFE_CANCEL:
      if (!isTerminating()) {
        terminate();
      } // if not terminating
      return ;
    default:
      _ASSERT(FALSE);
    } // switch
  } // if target file exists

  // Send request that we want to download file
  m_sender->sendDownloadRequest(m_pathToSourceFile.getString(), m_fileOffset);
}

void DownloadOperation::processFolder()
{
  File local(m_pathToTargetFile.getString());
  if (local.exists() && local.isDirectory()) {
  } else {
    if (!local.mkdir()) {
      // Logging
      StringStorage message;

      message.format(_T("Error: failed to create local folder '%s'"),
                     m_pathToTargetFile.getString());

      notifyError(message.getString());

      // Download next file
      gotoNext();
      return ;
    }
  }

  m_sender->sendFileListRequest(m_pathToSourceFile.getString(),
                                m_replyBuffer->isCompressionSupported());
}

void DownloadOperation::gotoNext()
{
  FileInfoList *current = m_toCopy;

  bool hasChild = current->getChild() != NULL;
  bool hasNext = current->getNext() != NULL;
  bool hasParent = current->getFirst()->getParent() != NULL;

  if (hasChild) {
    // If it has child, we must download child file list first
    changeFileToDownload(current->getChild());
    startDownload();
  } else if (hasNext) {
    // If it has no child, but has next file, we must download next file
    changeFileToDownload(current->getNext());
    startDownload();
  } else {

    //
    // If it has no child and not next, but has parent file,
    // we must go to parent file (folder i mean), set childs to NULL
    // cause we already download child files and go to next file.
    //

    FileInfoList *first = current->getFirst();
    if (hasParent) {
      changeFileToDownload(first->getParent());
      m_toCopy->setChild(NULL, 0);

      gotoNext();
    } else {
      killOp();
    } // if / else
  } // if / else
} // void

void DownloadOperation::tryCalcInputFilesSize()
{
  FileInfoList *fil = m_toCopy;

  while (fil != NULL) {
    if (fil->getFileInfo()->isDirectory()) {
      StringStorage pathNoRoot;
      StringStorage pathToFile;

      fil->getAbsolutePath(&pathNoRoot, _T('/'));

      pathToFile.setString(m_pathToSourceRoot.getString());
      if (!pathToFile.endsWith(_T('/'))) {
        pathToFile.appendString(_T("/"));
      }
      pathToFile.appendString(pathNoRoot.getString());

      m_foldersToCalcSizeLeft++;
      m_sender->sendFolderSizeRequest(pathToFile.getString());
    } else {
      m_totalBytesToCopy += fil->getFileInfo()->getSize();
    }
    fil = fil->getNext();
  }
}

void DownloadOperation::killOp()
{
  //
  // If not files to download, than clear memory and
  // operation is finished
  //

  delete m_toCopy->getRoot();
  m_toCopy = NULL;

  notifyFinish();
}

void DownloadOperation::decFoldersToCalcSizeCount()
{
  m_foldersToCalcSizeLeft--;

  // No more folders to calc size, start download
  if (m_foldersToCalcSizeLeft == 0) {
    startDownload();
  }
}

void DownloadOperation::notifyFailedToDownload(const TCHAR *errorDescription)
{
  StringStorage message;

  message.format(_T("Error: failed to download '%s' (%s)"),
                 m_pathToSourceFile.getString(),
                 errorDescription);

  notifyError(message.getString());
}

void DownloadOperation::changeFileToDownload(FileInfoList *toDownload)
{
  m_toCopy = toDownload;

  getRemotePath(m_toCopy, m_pathToSourceRoot.getString(), &m_pathToSourceFile);
  getLocalPath(m_toCopy, m_pathToTargetRoot.getString(), &m_pathToTargetFile);
}
