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

#ifndef _FILE_TRANSFER_REPLY_BUFFER_H_
#define _FILE_TRANSFER_REPLY_BUFFER_H_

#include "FileTransferEventHandler.h"

#include "io-lib/DataInputStream.h"

#include "ft-common/FileInfo.h"
#include "util/Inflater.h"
#include "util/ZLibException.h"

#include "ft-common/OperationNotSupportedException.h"

#include "log-writer/LogWriter.h"

class FileTransferReplyBuffer : public FileTransferEventHandler
{
public:
  FileTransferReplyBuffer(LogWriter *logWriter);
  virtual ~FileTransferReplyBuffer();

  void getLastErrorMessage(StringStorage *storage);

  bool isCompressionSupported();

  UINT32 getFilesInfoCount();
  FileInfo *getFilesInfo();

  UINT32 getDownloadBufferSize();
  vector<UINT8> getDownloadBuffer();

  UINT8 getDownloadFileFlags();
  UINT64 getDownloadLastModified();

  UINT64 getDirSize();

  //
  // Inherited from FileTransferEventHandler abstract class
  //

  virtual void onCompressionSupportReply(DataInputStream *input) throw(IOException);
  virtual void onFileListReply(DataInputStream *input) throw(IOException, ZLibException);
  virtual void onMd5DataReply(DataInputStream *input) throw(IOException, OperationNotSupportedException);

  virtual void onUploadReply(DataInputStream *input) throw(IOException);
  virtual void onUploadDataReply(DataInputStream *input) throw(IOException);
  virtual void onUploadEndReply(DataInputStream *input) throw(IOException);

  virtual void onDownloadReply(DataInputStream *input) throw(IOException);
  virtual void onDownloadDataReply(DataInputStream *input) throw(IOException, ZLibException);
  virtual void onDownloadEndReply(DataInputStream *input) throw(IOException);

  virtual void onMkdirReply(DataInputStream *input) throw(IOException);
  virtual void onRmReply(DataInputStream *input) throw(IOException);
  virtual void onMvReply(DataInputStream *input) throw(IOException);

  virtual void onDirSizeReply(DataInputStream *input) throw(IOException);
  virtual void onLastRequestFailedReply(DataInputStream *input) throw(IOException);

private:

  vector<UINT8> readCompressedDataBlock(DataInputStream *input,
                                        UINT32 compressedSize,
                                        UINT32 uncompressedSize,
                                        UINT8 compressionLevel)
                throw(IOException, ZLibException);

protected:

  //
  // Interface of log writer for log data
  //
  LogWriter *m_logWriter;

  //
  // ZLib stream for decompression of compressed data
  //
  Inflater m_inflater;

  //
  // Members than have access from public methods
  //

  // Compression support reply
  bool m_isCompressionSupported;

  // File list reply
  UINT32 m_filesInfoCount;
  FileInfo *m_filesInfo;

  // Last request message failed reply
  StringStorage m_lastErrorMessage;

  // Download data reply
  vector<UINT8> m_downloadBuffer;
  UINT32 m_downloadBufferSize;

  // Download end reply
  UINT8 m_downloadFileFlags;
  UINT64 m_downloadLastModified;

  // Dirsize reply data
  UINT64 m_dirSize;
};

#endif
