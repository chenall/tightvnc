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

#ifndef _REMOTE_FILES_DELETE_OPERATION_H_
#define _REMOTE_FILES_DELETE_OPERATION_H_

#include "FileTransferOperation.h"
#include "FileInfoList.h"

class RemoteFilesDeleteOperation : public FileTransferOperation
{
public:

  //
  // Constructs new RemoteFilesDeleteOperation class instance.
  //
  // Parameters:
  //
  // fileInfoToDelete - file information about file need to delete.
  // filename in this variable must be relative to pathToTargetRoot.
  //
  // pathToTargetRoot - path to folder on remote file system in ft format where
  // files to be deleted is located.
  //

  RemoteFilesDeleteOperation(LogWriter *logWriter,
                             const FileInfo *filesInfoToDelete,
                             size_t filesCount,
                             const TCHAR *pathToTargetRoot);

  RemoteFilesDeleteOperation(LogWriter *logWriter,
                             FileInfo fileInfoToDelete,
                             const TCHAR *pathToTargetRoot);

  virtual ~RemoteFilesDeleteOperation();

  //
  // Starts executing this delete operation
  //

  virtual void start() throw(IOException);

  //
  // File transfer message(accepted by this operation) handlers
  //

  void onFileListReply(DataInputStream *input) throw(IOException);
  void onRmReply(DataInputStream *input) throw(IOException);
  void onLastRequestFailedReply(DataInputStream *input) throw(IOException);

private:

  void remove(bool removeIfFolder) throw(IOException);
  void gotoNext() throw(IOException);
  void killOp();

protected:

  //
  // Current file list to delete
  //

  FileInfoList *m_toDelete;
  StringStorage m_pathToTargetRoot;
};

#endif
