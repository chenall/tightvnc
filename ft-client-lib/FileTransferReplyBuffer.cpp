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

#include "FileTransferReplyBuffer.h"
#include "io-lib/ByteArrayInputStream.h"
#include "thread/AutoLock.h"
#include <crtdbg.h>

FileTransferReplyBuffer::FileTransferReplyBuffer(LogWriter *logWriter)
: m_logWriter(logWriter),
  m_isCompressionSupported(false),
  m_filesInfoCount(0), m_filesInfo(NULL),
  m_downloadBufferSize(0), 
  m_downloadFileFlags(0), m_downloadLastModified(0),
  m_dirSize(0)
{
  m_lastErrorMessage.setString(_T(""));
}

FileTransferReplyBuffer::~FileTransferReplyBuffer()
{
  if (m_filesInfo != NULL) {
    delete[] m_filesInfo;
  }
}

void FileTransferReplyBuffer::getLastErrorMessage(StringStorage *storage)
{
  *storage = m_lastErrorMessage;
}

bool FileTransferReplyBuffer::isCompressionSupported()
{
  return m_isCompressionSupported;
}

UINT32 FileTransferReplyBuffer::getFilesInfoCount()
{
  return m_filesInfoCount;
}

FileInfo *FileTransferReplyBuffer::getFilesInfo()
{
  return m_filesInfo;
}

UINT32 FileTransferReplyBuffer::getDownloadBufferSize()
{
  return m_downloadBufferSize;
}

UINT8 FileTransferReplyBuffer::getDownloadFileFlags()
{
  return m_downloadFileFlags;
}

UINT64 FileTransferReplyBuffer::getDownloadLastModified()
{
  return m_downloadLastModified;
}

UINT64 FileTransferReplyBuffer::getDirSize()
{
  return m_dirSize;
}

vector<UINT8> FileTransferReplyBuffer::getDownloadBuffer()
{
  return m_downloadBuffer;
}

void FileTransferReplyBuffer::onCompressionSupportReply(DataInputStream *input)
{
  m_isCompressionSupported = (input->readUInt8() == 1);

  m_logWriter->info(_T("Received compression support reply: %s\n"),
                    m_isCompressionSupported ? _T("supported") : _T("not supported"));
}

void FileTransferReplyBuffer::onFileListReply(DataInputStream *input)
{
  UINT8 compressionLevel = 0;
  UINT32 compressedSize = 0;
  UINT32 uncompressedSize = 0;

  vector<UINT8> buffer;

  {
    compressionLevel = input->readUInt8();
    compressedSize = input->readUInt32();
    uncompressedSize = input->readUInt32();

    buffer = readCompressedDataBlock(input,
                                     compressedSize,
                                     uncompressedSize,
                                     compressionLevel);
  }

  if (!buffer.empty()) {
    // FIXME: type conversion in C-style
    ByteArrayInputStream memoryInputStream(reinterpret_cast<char *>(&buffer.front()),
                                           uncompressedSize);
    DataInputStream filesInfoReader(&memoryInputStream);

    if (m_filesInfo != 0) {
      delete[] m_filesInfo;
      m_filesInfo = 0;
    }

    m_filesInfoCount = filesInfoReader.readUInt32();
    if (m_filesInfoCount != 0) {
      m_filesInfo = new FileInfo[m_filesInfoCount];
    }

    for (UINT32 i = 0; i < m_filesInfoCount; i++) {
      FileInfo *fileInfo = &m_filesInfo[i];

      fileInfo->setSize(filesInfoReader.readUInt64());
      fileInfo->setLastModified(filesInfoReader.readUInt64());
      fileInfo->setFlags(filesInfoReader.readUInt16());

      StringStorage t;
      filesInfoReader.readUTF8(&t);

      fileInfo->setFileName(t.getString());
    } // for all newly created file's info

    m_logWriter->info(_T("Received file list reply: \n")
                      _T("\t files count = %d\n")
                      _T("\t use compression = %d\n"),
                      m_filesInfoCount, compressionLevel);
  } else {
    m_logWriter->info(_T("Received file list reply is not read: ")
                      _T("compressed buffer is empty"));
  }
}

void FileTransferReplyBuffer::onMd5DataReply(DataInputStream *input)
{
  throw OperationNotSupportedException();
}

void FileTransferReplyBuffer::onUploadReply(DataInputStream *input)
{
  m_logWriter->info(_T("Received upload reply\n"));
}

void FileTransferReplyBuffer::onUploadDataReply(DataInputStream *input)
{
  m_logWriter->info(_T("Received upload data reply\n"));
}

void FileTransferReplyBuffer::onUploadEndReply(DataInputStream *input)
{
  m_logWriter->info(_T("Received upload end reply\n"));
}

void FileTransferReplyBuffer::onDownloadReply(DataInputStream *input)
{
  m_logWriter->info(_T("Received download reply\n"));
}

void FileTransferReplyBuffer::onDownloadDataReply(DataInputStream *input)
{
  UINT8 coLevel = input->readUInt8();
  UINT32 coBufferSize = input->readUInt32();
  UINT32 uncoBufferSize = input->readUInt32();

  m_downloadBuffer = readCompressedDataBlock(input, coBufferSize, uncoBufferSize, coLevel);
  m_downloadBufferSize = uncoBufferSize;

  m_logWriter->info(_T("Received download data reply:\n")
                    _T("\tcompressed size: %d\n")
                    _T("\tuncompressed size: %d\n")
                    _T("\tuse compression: %d\n"),
                    coBufferSize, uncoBufferSize, coLevel);
}

void FileTransferReplyBuffer::onDownloadEndReply(DataInputStream *input)
{
  m_downloadFileFlags = input->readUInt8();
  m_downloadLastModified = input->readUInt64();

  m_logWriter->info(_T("Received download end reply:\n")
                    _T("\tfile flags: %d\n")
                    _T("\tmodification time: %ld\n"),
                    m_downloadFileFlags, m_downloadLastModified);
}

void FileTransferReplyBuffer::onMkdirReply(DataInputStream *input)
{
  m_logWriter->info(_T("Received mkdir reply\n"));
}

void FileTransferReplyBuffer::onRmReply(DataInputStream *input)
{
  m_logWriter->info(_T("Received rm reply\n"));
}

void FileTransferReplyBuffer::onMvReply(DataInputStream *input)
{
  m_logWriter->info(_T("Received rename reply\n"));
}

void FileTransferReplyBuffer::onDirSizeReply(DataInputStream *input)
{
  m_dirSize = input->readUInt64();

  m_logWriter->info(_T("Received dirsize reply\n"));
}

void FileTransferReplyBuffer::onLastRequestFailedReply(DataInputStream *input)
{
  input->readUTF8(&m_lastErrorMessage);

  m_logWriter->info(_T("Received last request failed reply:\n")
                    _T("\terror message: %s\n"),
                    m_lastErrorMessage.getString());
}

vector<UINT8> FileTransferReplyBuffer::readCompressedDataBlock(DataInputStream *input,
                                                               UINT32 compressedSize,
                                                               UINT32 uncompressedSize,
                                                               UINT8 compressionLevel)
{
  //
  // Buffers with compressed and uncompressed data.
  // When not using compression uncoBuffer = coBuffer.
  //

  UINT32 coSize = compressedSize;
  UINT32 uncoSize = uncompressedSize;

  vector<UINT8> coBuffer(coSize);

  //
  // Read compressed data
  //

  if (coSize == 0) {
    return coBuffer;
  }

  input->readFully(&coBuffer.front(), coSize);

  if (compressionLevel == 0) {
    return coBuffer;
  }

  vector<UINT8> uncoBuffer(uncoSize);

  m_inflater.setUnpackedSize(uncoSize);
  // FIXME: type conversion in C-style
  m_inflater.setInput((const char*)&coBuffer.front(), coSize);

  m_inflater.inflate();

  _ASSERT(m_inflater.getOutputSize() == uncoSize);

  if (uncoSize != 0) {
    memcpy(&uncoBuffer.front(), m_inflater.getOutput(), uncoSize);
  }

  return uncoBuffer;
}
