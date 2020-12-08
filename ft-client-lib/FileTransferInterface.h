// Copyright (C) 2012 GlavSoft LLC.
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

#ifndef _FILE_TRANSFER_INTERFACE_H_
#define _FILE_TRANSFER_INTERFACE_H_

#include "FileTransferCore.h"

class FileTransferCore;

class FileTransferInterface
{
public:
  FileTransferInterface(FileTransferCore *core);
  virtual ~FileTransferInterface();

  virtual int onFtTargetFileExists(FileInfo *sourceFileInfo,
                                   FileInfo *targetFileInfo,
                                   const TCHAR *pathToTargetFile) = 0;
  // Progress is in interval [0.0, 1.0].
  virtual void setProgress(double progress) = 0;

  //
  // This function inform user about error.
  // This function must be is not blocking, otherwise it may happen deadlock.
  //
  virtual void onFtOpError(const TCHAR *message) = 0;

  //
  // This function inform user additional information.
  // This function must be is not blocking, otherwise it may happen deadlock.
  //
  virtual void onFtOpInfo(const TCHAR *message) = 0;
  virtual void onFtOpStarted() = 0;
  virtual void onFtOpFinished(int state, int result) = 0;

  //
  // filetransfer's operation is finished. Need update of control
  //
  virtual void setNothingState() = 0;

  //
  // Called if local file list is updated
  //
  virtual void onRefreshLocalFileList() = 0;

  // Called if remote file list is updated
  virtual void onRefreshRemoteFileList() = 0;

  //
  // Shows error message and throws exception
  //

  virtual void raise(Exception &ex) = 0;

protected:
  FileTransferCore *m_ftCore;
};

#endif
