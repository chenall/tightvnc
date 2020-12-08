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

#include "FileTransferRequestHandler.h"

#include "io-lib/DataOutputStream.h"

#include "ft-common/FileTransferException.h"
#include "io-lib/ByteArrayOutputStream.h"
#include "file-lib/File.h"
#include "file-lib/EOFException.h"
#include "ft-common/FolderListener.h"
#include "ft-common/FTMessage.h"
#include "ft-common/WinFilePath.h"
#include "ft-common/FileInfo.h"
#include "util/md5.h"
#include "network/RfbOutputGate.h"
#include "network/RfbInputGate.h"
#include "thread/AutoLock.h"
#include "win-system/Impersonator.h"
#include "win-system/Environment.h"
#include "server-config-lib/Configurator.h"
#include "win-system/SystemException.h"
#include "rfb/VendorDefs.h"

FileTransferRequestHandler::FileTransferRequestHandler(RfbCodeRegistrator *registrator,
                                                       RfbOutputGate *output,
                                                       Desktop *desktop,
                                                       LogWriter *log,
                                                       bool enabled)
: m_downloadFile(NULL), m_fileInputStream(NULL),
  m_uploadFile(NULL), m_fileOutputStream(NULL),
  m_output(output), m_enabled(enabled),
  m_log(log)
{
  m_security = new FileTransferSecurity(desktop, m_log);

  if (!FileTransferRequestHandler::isFileTransferEnabled()) {
    return ;
  }

  registrator->addSrvToClCap(FTMessage::COMPRESSION_SUPPORT_REPLY, VendorDefs::TIGHTVNC, FTMessage::COMPRESSION_SUPPORT_REPLY_SIG);
  registrator->addSrvToClCap(FTMessage::FILE_LIST_REPLY, VendorDefs::TIGHTVNC, FTMessage::FILE_LIST_REPLY_SIG);
  registrator->addSrvToClCap(FTMessage::MD5_REPLY, VendorDefs::TIGHTVNC, FTMessage::MD5_REPLY_SIG);
  registrator->addSrvToClCap(FTMessage::UPLOAD_START_REPLY, VendorDefs::TIGHTVNC, FTMessage::UPLOAD_START_REPLY_SIG);
  registrator->addSrvToClCap(FTMessage::UPLOAD_DATA_REPLY, VendorDefs::TIGHTVNC, FTMessage::UPLOAD_DATA_REPLY_SIG);
  registrator->addSrvToClCap(FTMessage::UPLOAD_END_REPLY, VendorDefs::TIGHTVNC, FTMessage::UPLOAD_END_REPLY_SIG);
  registrator->addSrvToClCap(FTMessage::DOWNLOAD_START_REPLY, VendorDefs::TIGHTVNC, FTMessage::DOWNLOAD_START_REPLY_SIG);
  registrator->addSrvToClCap(FTMessage::DOWNLOAD_DATA_REPLY, VendorDefs::TIGHTVNC, FTMessage::DOWNLOAD_DATA_REPLY_SIG);
  registrator->addSrvToClCap(FTMessage::DOWNLOAD_END_REPLY, VendorDefs::TIGHTVNC, FTMessage::DOWNLOAD_END_REPLY_SIG);
  registrator->addSrvToClCap(FTMessage::MKDIR_REPLY, VendorDefs::TIGHTVNC, FTMessage::MKDIR_REPLY_SIG);
  registrator->addSrvToClCap(FTMessage::REMOVE_REPLY, VendorDefs::TIGHTVNC, FTMessage::REMOVE_REPLY_SIG);
  registrator->addSrvToClCap(FTMessage::RENAME_REPLY, VendorDefs::TIGHTVNC, FTMessage::RENAME_REPLY_SIG);
  registrator->addSrvToClCap(FTMessage::DIRSIZE_REPLY, VendorDefs::TIGHTVNC, FTMessage::DIRSIZE_REPLY_SIG);
  registrator->addSrvToClCap(FTMessage::LAST_REQUEST_FAILED_REPLY, VendorDefs::TIGHTVNC, FTMessage::LAST_REQUEST_FAILED_REPLY_SIG);

  registrator->addClToSrvCap(FTMessage::COMPRESSION_SUPPORT_REQUEST, VendorDefs::TIGHTVNC, FTMessage::COMPRESSION_SUPPORT_REQUEST_SIG);
  registrator->addClToSrvCap(FTMessage::FILE_LIST_REQUEST, VendorDefs::TIGHTVNC, FTMessage::FILE_LIST_REQUEST_SIG);
  registrator->addClToSrvCap(FTMessage::MD5_REQUEST, VendorDefs::TIGHTVNC, FTMessage::MD5_REQUEST_SIG);
  registrator->addClToSrvCap(FTMessage::UPLOAD_START_REQUEST, VendorDefs::TIGHTVNC, FTMessage::UPLOAD_START_REQUEST_SIG);
  registrator->addClToSrvCap(FTMessage::UPLOAD_DATA_REQUEST, VendorDefs::TIGHTVNC, FTMessage::UPLOAD_DATA_REQUEST_SIG);
  registrator->addClToSrvCap(FTMessage::UPLOAD_END_REQUEST, VendorDefs::TIGHTVNC, FTMessage::UPLOAD_END_REQUEST_SIG);
  registrator->addClToSrvCap(FTMessage::DOWNLOAD_START_REQUEST, VendorDefs::TIGHTVNC, FTMessage::DOWNLOAD_START_REQUEST_SIG);
  registrator->addClToSrvCap(FTMessage::DOWNLOAD_DATA_REQUEST, VendorDefs::TIGHTVNC, FTMessage::DOWNLOAD_DATA_REQUEST_SIG);
  registrator->addClToSrvCap(FTMessage::MKDIR_REQUEST, VendorDefs::TIGHTVNC, FTMessage::MKDIR_REQUEST_SIG);
  registrator->addClToSrvCap(FTMessage::REMOVE_REQUEST, VendorDefs::TIGHTVNC, FTMessage::REMOVE_REQUEST_SIG);
  registrator->addClToSrvCap(FTMessage::RENAME_REQUEST, VendorDefs::TIGHTVNC, FTMessage::RENAME_REQUEST_SIG);
  registrator->addClToSrvCap(FTMessage::DIRSIZE_REQUEST, VendorDefs::TIGHTVNC, FTMessage::DIRSIZE_REQUEST_SIG);

  UINT32 rfbMessagesToProcess[] = {
    FTMessage::COMPRESSION_SUPPORT_REQUEST,
    FTMessage::FILE_LIST_REQUEST,
    FTMessage::MD5_REQUEST,
    FTMessage::UPLOAD_START_REQUEST,
    FTMessage::UPLOAD_DATA_REQUEST,
    FTMessage::UPLOAD_END_REQUEST,
    FTMessage::DOWNLOAD_START_REQUEST,
    FTMessage::DOWNLOAD_DATA_REQUEST,
    FTMessage::MKDIR_REQUEST,
    FTMessage::REMOVE_REQUEST,
    FTMessage::RENAME_REQUEST,
    FTMessage::DIRSIZE_REQUEST
  };

  for (size_t i = 0; i < sizeof(rfbMessagesToProcess) / sizeof(UINT32); i++) {
    registrator->regCode(rfbMessagesToProcess[i], this);
  }

  m_log->message(_T("File transfer request handler created"));
}

FileTransferRequestHandler::~FileTransferRequestHandler()
{
  delete m_security;

  if (m_fileInputStream != NULL) {
    delete m_fileInputStream;
  }
  if (m_downloadFile != NULL) {
    delete m_downloadFile;
  }
  if (m_fileOutputStream != NULL) {
    delete m_fileOutputStream;
  }
  if (m_uploadFile != NULL) {
    delete m_uploadFile;
  }

  m_log->message(_T("File transfer request handler deleted"));
}

void FileTransferRequestHandler::onRequest(UINT32 reqCode, RfbInputGate *backGate)
{
  m_security->beginMessageProcessing();

  m_input = backGate;

  try {
    switch (reqCode) {
    case FTMessage::COMPRESSION_SUPPORT_REQUEST:
      compressionSupportRequested();
      break;
    case FTMessage::FILE_LIST_REQUEST:
      fileListRequested();
      break;
    case FTMessage::MKDIR_REQUEST:
      mkDirRequested();
      break;
    case FTMessage::REMOVE_REQUEST:
      rmFileRequested();
      break;
    case FTMessage::RENAME_REQUEST:
      mvFileRequested();
      break;
    case FTMessage::DIRSIZE_REQUEST:
      dirSizeRequested();
      break;
    case FTMessage::UPLOAD_START_REQUEST:
      uploadStartRequested();
      break;
    case FTMessage::UPLOAD_DATA_REQUEST:
      uploadDataRequested();
      break;
    case FTMessage::UPLOAD_END_REQUEST:
      uploadEndRequested();
      break;
    case FTMessage::DOWNLOAD_START_REQUEST:
      downloadStartRequested();
      break;
    case FTMessage::DOWNLOAD_DATA_REQUEST:
      downloadDataRequested();
      break;
    case FTMessage::MD5_REQUEST:
      md5Requested();
      break;
    } // switch.
  } catch (Exception &someEx) {
    lastRequestFailed(someEx.getMessage());
  } // try / catch.

  m_input = NULL;

  m_security->endMessageProcessing();
}

bool FileTransferRequestHandler::isFileTransferEnabled()
{
  return m_enabled && Configurator::getInstance()->getServerConfig()->isFileTransfersEnabled();
}

void FileTransferRequestHandler::compressionSupportRequested()
{
  m_log->message(_T("%s"), _T("compression support requested"));

  //
  // Can be 0 - compression not supported by server
  //     or 1 - compression is supported by server
  //

  UINT8 compressionSupport = 1;

  m_log->debug(_T("sending compression support reply: %s"), (compressionSupport == 1) ? _T("supported") : _T("not supported"));

  {
    AutoLock l(m_output);

    m_output->writeUInt32(FTMessage::COMPRESSION_SUPPORT_REPLY);
    m_output->writeUInt8(compressionSupport);

    m_output->flush();
  }
}

void FileTransferRequestHandler::fileListRequested()
{
  UINT8 requestedCompressionLevel;
  WinFilePath fullPathName;

  //
  // Read input data
  //

  {
    requestedCompressionLevel = m_input->readUInt8();

    m_input->readUTF8(&fullPathName);
  }

  m_log->message(_T("File list of folder '%s' requested"),
               fullPathName.getString());

  checkAccess();

  UINT8 compressionLevel = requestedCompressionLevel;
  UINT32 compressedSize = 0;
  UINT32 uncompressedSize = 0;
  UINT32 filesCount = 0;
  UINT32 filesInfoDataSize = 0;
  const FileInfo *files = NULL;

  //
  // Get file list from specified folder
  //

  FolderListener folderListener(fullPathName.getString());

  if (!folderListener.list()) {
    throw SystemException();
  }

  files = folderListener.getFilesInfo();
  filesCount = folderListener.getFilesCount();

  //
  // Create buffer with "CompressedData" block inside
  //

  ByteArrayOutputStream memStream;
  DataOutputStream outMemStream(&memStream);

  outMemStream.writeUInt32(filesCount);
  for (UINT32 i = 0; i < filesCount; i++) {
    outMemStream.writeUInt64(files[i].getSize());
    outMemStream.writeUInt64(files[i].lastModified());
    outMemStream.writeUInt16(files[i].getFlags());
    outMemStream.writeUTF8(files[i].getFileName());
  } // for

  _ASSERT((UINT32)memStream.size() == memStream.size());
  uncompressedSize = (UINT32)memStream.size();

  //
  // Buffer for data in "CompressedData" block
  //

  compressedSize = uncompressedSize;

  if (compressionLevel != 0) {
    m_deflater.setInput(memStream.toByteArray(), memStream.size());
    m_deflater.deflate();
    _ASSERT((UINT32)m_deflater.getOutputSize() == m_deflater.getOutputSize());
    compressedSize = (UINT32)m_deflater.getOutputSize();
  }

  //
  // Write data to socket
  //

  {
    AutoLock l(m_output);

    m_output->writeUInt32(FTMessage::FILE_LIST_REPLY);

    m_output->writeUInt8(compressionLevel);
    m_output->writeUInt32(compressedSize);
    m_output->writeUInt32(uncompressedSize);

    if (compressionLevel != 0) {
      m_output->writeFully(m_deflater.getOutput(), compressedSize);
    } else {
      m_output->writeFully(memStream.toByteArray(), uncompressedSize);
    }

    m_output->flush();
  } // synchronized(m_output)
} // void

void FileTransferRequestHandler::mkDirRequested()
{
  WinFilePath folderPath;

  {
    m_input->readUTF8(&folderPath);
  } // end of reading block.

  m_log->message(_T("mkdir \"%s\" command requested"), folderPath.getString());

  checkAccess();

  if (folderPath.parentPathIsRoot()) {
    throw FileTransferException(_T("Cannot create folder in root folder"));
  }

  File folder(folderPath.getString());

  if (folder.exists()) {
    throw FileTransferException(_T("Directory already exists"));
  }

  if (!folder.mkdir()) {
    throw SystemException();
  }

  {
    AutoLock l(m_output);

    m_output->writeUInt32(FTMessage::MKDIR_REPLY);

    m_output->flush();
  }
}

void FileTransferRequestHandler::rmFileRequested()
{
  WinFilePath fullPathName;

  {
    m_input->readUTF8(&fullPathName);
  } // end of reading block.

  m_log->message(_T("rm \"%s\" command requested"), fullPathName.getString());

  checkAccess();

  File file(fullPathName.getString());

  if (!file.exists()) {
    throw SystemException();
  } // file file does not exists

  if (!file.remove()) {
    throw SystemException();
  } // if cannot delete file

  {
    AutoLock l(m_output);

    m_output->writeUInt32(FTMessage::REMOVE_REPLY);

    m_output->flush();
  }
} // void

void FileTransferRequestHandler::mvFileRequested()
{
  WinFilePath oldFileName;
  WinFilePath newFileName;

  {
    m_input->readUTF8(&oldFileName);
    m_input->readUTF8(&newFileName);
  } // end of reading block.

  m_log->message(_T("move \"%s\" \"%s\" command requested"), oldFileName.getString(), newFileName.getString());

  checkAccess();

  File srcFile(oldFileName.getString());
  File dstFile(newFileName.getString());

  if (!srcFile.renameTo(&dstFile)) {
    throw SystemException();
  }

  {
    AutoLock l(m_output);

    m_output->writeUInt32(FTMessage::RENAME_REPLY);

    m_output->flush();
  }
}

void FileTransferRequestHandler::dirSizeRequested()
{
  WinFilePath fullPathName;

  {
    m_input->readUTF8(&fullPathName);
  } // end of reading block.

  m_log->message(_T("Size of folder '%s\' requested"),
               fullPathName.getString());

  checkAccess();

  UINT64 directorySize = 0;

  if (!getDirectorySize(fullPathName.getString(), &directorySize)) {
    throw SystemException();
  }

  {
    AutoLock l(m_output);

    m_output->writeUInt32(FTMessage::DIRSIZE_REPLY);
    m_output->writeUInt64(directorySize);

    m_output->flush();
  }
} // void

void FileTransferRequestHandler::md5Requested()
{
  WinFilePath fullPathName;

  UINT64 offset;
  UINT64 dataLen;

  {
    m_input->readUTF8(&fullPathName);

    offset = m_input->readUInt64();
    dataLen = m_input->readUInt64();
  } // end of reading block.

  m_log->message(_T("md5 \"%s\" %d %d command requested"), fullPathName.getString(), offset, dataLen);

  checkAccess();

  //
  // Action
  //

  MD5 md5calculator;

  File file(fullPathName.getString());

  StringStorage path;
  file.getPath(&path);
  WinFileChannel fileInputStream(path.getString(), F_READ, FM_OPEN);
  fileInputStream.seek(offset);

  //
  // Begin reading needed file data
  //

  DWORD bytesToRead = 1024 * 1024;
  UINT64 bytesToReadTotal = dataLen;
  size_t bytesRead = 0;
  UINT64 bytesReadTotal = 0;

  if (dataLen < (UINT64)bytesToRead) {
    bytesToRead = (DWORD)dataLen;
  }

  std::vector<UINT8> buffer(bytesToRead);

  while (bytesToReadTotal > 0) {
    bytesRead = fileInputStream.read(&buffer.front(), bytesToRead);
    bytesReadTotal += bytesRead;
    bytesToReadTotal -= bytesRead;

    if (bytesToReadTotal < (UINT64)bytesToRead) {
      bytesToRead = (DWORD)bytesToReadTotal;
    }

    md5calculator.update(&buffer.front(), (UINT32)bytesRead);
  } // while

  md5calculator.finalize();

  {
    AutoLock l(m_output);

    m_output->writeUInt32(FTMessage::MD5_REPLY);
    m_output->writeFully((const char *)md5calculator.getHash(), 16);

    m_output->flush();
  }
}

void FileTransferRequestHandler::uploadStartRequested()
{
  //
  // Request input variables.
  //

  WinFilePath fullPathName;
  UINT8 uploadFlags;
  UINT64 initialOffset;

  {
    m_input->readUTF8(&fullPathName);
    uploadFlags = m_input->readUInt8();
    initialOffset = m_input->readUInt64();
  }

  m_log->message(_T("upload \"%s\" %d %d command requested"), fullPathName.getString(), uploadFlags, initialOffset);

  checkAccess();

  //
  // Closing previous upload if it was broken
  //

  if (m_fileOutputStream != NULL) {
    delete m_fileOutputStream;
    m_fileOutputStream = 0;
  }
  if (m_uploadFile != NULL) {
    delete m_uploadFile;
    m_uploadFile = 0;
  }

  if (fullPathName.parentPathIsRoot()) {
    throw FileTransferException(_T("Cannot upload file to root folder"));
  }

  m_uploadFile = new File(fullPathName.getString());

  //
  // Trying to create file or overwrite existing
  //

  if ((uploadFlags & 0x1) && (!m_uploadFile->truncate())) {
    throw SystemException();
  }

  //
  // Trying to open file and seek to initial file position
  //
  m_fileOutputStream = new WinFileChannel(fullPathName.getString(),
                                          F_WRITE,
                                          FM_OPEN);
  m_fileOutputStream->seek(initialOffset);

  //
  // Send reply
  //

  {
    AutoLock l(m_output);

    m_output->writeUInt32(FTMessage::UPLOAD_START_REPLY);

    m_output->flush();
  }
} // void

void FileTransferRequestHandler::uploadDataRequested()
{
  //
  // Request input variables.
  //

  UINT8 compressionLevel;
  UINT32 compressedSize;
  UINT32 uncompressedSize;

  compressionLevel = m_input->readUInt8();
  compressedSize = m_input->readUInt32();
  uncompressedSize = m_input->readUInt32();
  std::vector<char> buffer(compressedSize);
  if (compressedSize != 0) {
    m_input->readFully(&buffer.front(), compressedSize);
  }

  m_log->info(_T("upload data (cs = %d, us = %d) requested"), compressedSize, uncompressedSize);

  checkAccess();

  if (m_uploadFile == NULL) {
    throw FileTransferException(_T("No active upload at the moment"));
  }

  if (compressedSize != 0) {
    if (compressionLevel == 0) {
      DataOutputStream dataOutStream(m_fileOutputStream);
      dataOutStream.writeFully(&buffer.front(), uncompressedSize);
    } else {
      m_inflater.setInput(&buffer.front(), compressedSize);
      m_inflater.setUnpackedSize(uncompressedSize);
      m_inflater.inflate();

      DataOutputStream dataOutStream(m_fileOutputStream);

      dataOutStream.writeFully(m_inflater.getOutput(), m_inflater.getOutputSize());
    } // if using compression
  }

  {
    AutoLock l(m_output);

    m_output->writeUInt32(FTMessage::UPLOAD_DATA_REPLY);

    m_output->flush();
  }
}

void FileTransferRequestHandler::uploadEndRequested()
{
  UINT16 fileFlags;
  UINT64 modificationTime;

  {
    fileFlags = m_input->readUInt16();
    modificationTime = m_input->readUInt64();
  } // end of reading block.

  m_log->message(_T("%s"), _T("end of upload requested\n"));

  checkAccess();

  //
  // No active uploads at the moment.
  // Client is "bad" if send to us this message
  //

  if (m_uploadFile == NULL) {
    throw FileTransferException(_T("No active upload at the moment"));
  }

  //
  // Close file output stream
  //

  try {
    m_fileOutputStream->close();
  } catch (...) { }

  //
  // Trying to set modification time
  //

  if (!m_uploadFile->setLastModified(modificationTime)) {
    throw FileTransferException(_T("Cannot change last write file time"));
  } // if cannot set modification time

  //
  // Send reply
  //

  {
    AutoLock l(m_output);

    m_output->writeUInt32(FTMessage::UPLOAD_END_REPLY);

    m_output->flush();
  }

  //
  // Cleanup
  //

  if (m_fileOutputStream != NULL) {
    delete m_fileOutputStream;
    m_fileOutputStream = NULL;
  }

  if (m_uploadFile != NULL) {
    delete m_uploadFile;
    m_uploadFile = NULL;
  }

} // void

void FileTransferRequestHandler::downloadStartRequested()
{
  WinFilePath fullPathName;
  UINT64 initialOffset;

  //
  // Reading command arguments
  //

  {
    m_input->readUTF8(&fullPathName);
    initialOffset = m_input->readUInt64();
  } // end of reading block.

  m_log->message(_T("download of \"%s\" file (offset = %d) requested"), fullPathName.getString(), initialOffset);

  checkAccess();

  //
  // Ending previous download if it was broken
  //

  if (m_fileInputStream != 0) {
    delete m_fileInputStream;
    m_fileInputStream = 0;
  }
  if (m_downloadFile != 0) {
    delete m_downloadFile;
    m_downloadFile = 0;
  }

  m_downloadFile = new File(fullPathName.getString());


  //
  // Try to open file for reading and seek to initial
  // file position.
  //

  m_fileInputStream = new WinFileChannel(fullPathName.getString(), F_READ,
                                         FM_OPEN);
  m_fileInputStream->seek(initialOffset);

  {
    AutoLock l(m_output);

    m_output->writeUInt32(FTMessage::DOWNLOAD_START_REPLY);

    m_output->flush();
  }
}

//
// FIXME: file flags is unimplemented
//

void FileTransferRequestHandler::downloadDataRequested()
{
  //
  // Request input variables.
  //

  UINT8 requestedCompressionLevel;
  UINT32 dataSize;

  {
    requestedCompressionLevel = m_input->readUInt8();
    dataSize = m_input->readUInt32();
  } // end of reading block.

  m_log->info(_T("download %d bytes (comp flag = %d) requested"), dataSize, requestedCompressionLevel);

  checkAccess();

  //
  // Data download reply variables.
  //

  UINT8 compressionLevel = requestedCompressionLevel;
  UINT32 compressedSize;
  UINT32 uncompressedSize;

  //
  // Client cannot send this request cause there is no
  // active download at the moment
  //

  if (m_downloadFile == NULL) {
    throw FileTransferException(_T("No active download at the moment"));
  }

  std::vector<char> buffer(dataSize);

  DWORD read = 0;

  try {
    if (dataSize != 0) {
      size_t portion = m_fileInputStream->read(&buffer.front(), dataSize);
      read = (DWORD)portion;
      _ASSERT(read == portion);
    }
  } catch (EOFException) {

    //
    // End of file detected
    //

    try { m_fileInputStream->close(); } catch (...) { }

    UINT8 fileFlags = 0;

    {
      AutoLock l(m_output);

      m_output->writeUInt32(FTMessage::DOWNLOAD_END_REPLY);
      m_output->writeUInt8(fileFlags);
      m_output->writeUInt64(m_downloadFile->lastModified());

      m_output->flush();
    } // rfb io handle block

    m_log->message(_T("%s"), _T("downloading has finished\n"));

    delete m_fileInputStream;
    delete m_downloadFile;

    m_fileInputStream = NULL;
    m_downloadFile = NULL;

    return ;

  } catch (IOException &ioEx) {
    throw FileTransferException(&ioEx);
  } // try / catch

  compressedSize = read;
  uncompressedSize = read;

  if (compressionLevel != 0) {
    if (dataSize != 0) {
      m_deflater.setInput(&buffer.front(), uncompressedSize);
      m_deflater.deflate();
      _ASSERT((UINT32)m_deflater.getOutputSize() == m_deflater.getOutputSize());
      compressedSize = (UINT32)m_deflater.getOutputSize();
    }
  }

  //
  // Send download data reply
  //

  AutoLock l(m_output);

  m_output->writeUInt32(FTMessage::DOWNLOAD_DATA_REPLY);
  m_output->writeUInt8(compressionLevel);
  m_output->writeUInt32(compressedSize);
  m_output->writeUInt32(uncompressedSize);

  if (compressionLevel == 0) {
    if (dataSize != 0) {
      m_output->writeFully(&buffer.front(), uncompressedSize);
    }
  } else {
    m_output->writeFully((const char *)m_deflater.getOutput(), compressedSize);
  }

  m_output->flush();
}

void FileTransferRequestHandler::lastRequestFailed(StringStorage *storage)
{
  lastRequestFailed(storage->getString());
}

void FileTransferRequestHandler::lastRequestFailed(const TCHAR *description)
{
  m_log->error(_T("last request failed: \"%s\""), description);

  {
    AutoLock l(m_output);

    m_output->writeUInt32(FTMessage::LAST_REQUEST_FAILED_REPLY);
    m_output->writeUTF8(description);

    m_output->flush();
  }
}

bool FileTransferRequestHandler::getDirectorySize(const TCHAR *pathname, UINT64 *dirSize)
{
  UINT64 currentDirSize = 0;
  UINT32 filesCount = 0;
  UINT32 dataSize = 0;

  File folder(pathname);

  //
  // Get files count
  //

  if (!folder.list(NULL, &filesCount)) {
    return false;
  }

  if (filesCount != 0) {
    std::vector<StringStorage> fileNames(filesCount);

    //
    // Get file names
    //

    folder.list(&fileNames.front(), NULL);

    for (UINT32 i = 0; i < filesCount; i++) {
      File subfile(pathname, fileNames[i].getString());
      if (subfile.isDirectory()) {

        UINT64 subDirSize = 0;
        StringStorage subDirPath;

        subfile.getPath(&subDirPath);

        if (getDirectorySize(subDirPath.getString(), &subDirSize)) {
          currentDirSize += subDirSize;
        }  // if it got sub directory size
      } else {
        currentDirSize += subfile.length();
      } // if subfile is normal file
    } // for every subfile in file list
  }
  *dirSize = currentDirSize;

  return true;
}

void FileTransferRequestHandler::checkAccess()
{
  try {
    if (!isFileTransferEnabled()) {
      throw Exception(_T("File transfers is disabled"));
    }
    m_security->throwIfAccessDenied();
  } catch (Exception &someEx) {
    throw SystemException(someEx.getMessage());
  } // try / catch.
}
