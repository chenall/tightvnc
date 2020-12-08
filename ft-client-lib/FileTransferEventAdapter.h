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

#ifndef _FILE_TRANSFER_EVENT_ADAPTER_H_
#define _FILE_TRANSFER_EVENT_ADAPTER_H_

#include "util/inttypes.h"
#include "FileTransferEventHandler.h"
#include "OperationNotPermittedException.h"

#include <vector>

using namespace std;

//
// Class is used as helper for checking valid (file transfer client point of view)
// file transfer protocol extension messages.
//
// This class throws OperationNotPermittedException on every file transfer
// message event handler. Child class must override methods to allow messages.
//

class FileTransferEventAdapter : public FileTransferEventHandler
{
public:
  FileTransferEventAdapter();
  virtual ~FileTransferEventAdapter();

  virtual void onCompressionSupportReply(DataInputStream *input) throw(OperationNotPermittedException);
  virtual void onFileListReply(DataInputStream *input) throw(OperationNotPermittedException);
  virtual void onMd5DataReply(DataInputStream *input) throw(OperationNotPermittedException);

  virtual void onUploadReply(DataInputStream *input) throw(OperationNotPermittedException);
  virtual void onUploadDataReply(DataInputStream *input) throw(OperationNotPermittedException);
  virtual void onUploadEndReply(DataInputStream *input) throw(OperationNotPermittedException);

  virtual void onDownloadReply(DataInputStream *input) throw(OperationNotPermittedException);
  virtual void onDownloadDataReply(DataInputStream *input) throw(OperationNotPermittedException);
  virtual void onDownloadEndReply(DataInputStream *input) throw(OperationNotPermittedException);

  virtual void onMkdirReply(DataInputStream *input) throw(OperationNotPermittedException);
  virtual void onRmReply(DataInputStream *input) throw(OperationNotPermittedException);
  virtual void onMvReply(DataInputStream *input) throw(OperationNotPermittedException);

  virtual void onDirSizeReply(DataInputStream *input) throw(OperationNotPermittedException);
  virtual void onLastRequestFailedReply(DataInputStream *input) throw(OperationNotPermittedException);
};

#endif
