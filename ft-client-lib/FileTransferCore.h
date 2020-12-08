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

#ifndef _FILE_TRANSFER_CORE_H_
#define _FILE_TRANSFER_CORE_H_

#include "ft-common/FileInfo.h"
#include "log-writer/LogWriter.h"

#include "FileTransferEventHandler.h"
#include "FileTransferRequestSender.h"
#include "FileTransferReplyBuffer.h"
#include "FileTransferOperation.h"
#include "OperationEventListener.h"
#include "CopyFileEventListener.h"
#include "OperationSupport.h"

#include "FileTransferInterface.h"

class FileTransferInterface;

class FileTransferCore : public OperationEventListener,
                         public CopyFileEventListener
{
public:
  //
  // Creates new instance of class.
  //
  // Parameters:
  // sender - object that needs for file transfer operations
  // replyBuffer - object that needs for file transfer operations
  // ftListeners - needs for executing file transfer operations
  //

  FileTransferCore(LogWriter *logWriter,
                   FileTransferRequestSender *sender,
                   FileTransferReplyBuffer *replyBuffer,
                   ListenerContainer<FileTransferEventHandler *> *ftListeners);
  virtual ~FileTransferCore();

  void setInterface(FileTransferInterface *ftInterface);

  const OperationSupport &getSupportedOps();

  vector<FileInfo> *getListLocalFolder(const TCHAR *pathToFile);
  vector<FileInfo> *getListRemoteFolder();

  void downloadOperation(const FileInfo *filesToDownload,
                         size_t filesCount,
                         const TCHAR *pathToTargetRoot,
                         const TCHAR *pathToSourceRoot);
  void uploadOperation(const FileInfo *filesToDownload,
                       size_t filesCount,
                       const TCHAR *pathToSourceRoot,
                       const TCHAR *pathToTargetRoot);
  void localFilesDeleteOperation(const FileInfo *filesToDelete,
                                 UINT32 filesCount,
                                 const TCHAR *pathToTargetRoot);
  void remoteFilesDeleteOperation(const FileInfo *filesInfoToDelete,
                                  size_t filesCount,
                                  const TCHAR *pathToTargetRoot);
  void remoteFolderCreateOperation(FileInfo file, const TCHAR *pathToTargetRoot);
  void remoteFileRenameOperation(FileInfo sourceFileInfo,
                                 FileInfo targetFileInfo,
                                 const TCHAR *pathToTargetRoot);
  void remoteFileListOperation(const TCHAR *pathToFile);

  void terminateCurrentOperation();

  bool isNothingState();

  void onOperationFinished();
  void onUpdateState(int state, int result);

  // FIXME: Debug.
  void updateSupportedOperations(const vector<UINT32> *clientCaps,
                                 const vector<UINT32> *serverCaps);

protected:
  //
  // Inherited from CopyFileEventListener
  //

  virtual void dataChunkCopied(UINT64 totalBytesCopied, UINT64 totalBytesToCopy);

  virtual int targetFileExists(FileInfo *sourceFileInfo,
                               FileInfo *targetFileInfo,
                               const TCHAR *pathToTargetFile);

  //
  // Inherited from OperationEventListener
  //

  virtual void ftOpStarted(FileTransferOperation *sender);
  virtual void ftOpFinished(FileTransferOperation *sender) throw(IOException);
  virtual void ftOpErrorMessage(FileTransferOperation *sender, const TCHAR *message);
  virtual void ftOpInfoMessage(FileTransferOperation *sender,
                               const TCHAR *message);

  //
  // Interface of log
  //

  LogWriter *m_logWriter;

  //
  // Current dialog state, needed for checking what type of
  // file transfer operation is executing now (means nothing or
  // delete, upload, download etc).
  //

  int m_state;

  //
  // Current file transfer operation.
  // Remark: only one file operation can be executed at one time
  //

  FileTransferOperation *m_currentOperation;
  //
  // File transfer message listener container.
  //
  // This member needs for executing file transfer operations,
  // after operation is created, but before it has started it needs
  // to be added to these listeners, after it has finished work needs
  // to remove operation from listeners list.
  //

  ListenerContainer<FileTransferEventHandler *> *m_fileTransferListeners;
  //
  // Request sender, reply buffer
  //

  FileTransferRequestSender *m_sender;
  FileTransferReplyBuffer *m_replyBuffer;

  //
  // Class that knows about supported operations.
  //

  OperationSupport m_supportedOps;

  //
  // File list request variables
  //

  vector <FileInfo> m_remoteFilesInfo;

  //
  // Local file list variables
  //

  vector <FileInfo> m_localFilesInfo;

  //
  // file transfer interface
  //
  FileTransferInterface *m_ftInterface;

  //
  // Frees previous file transfer operation if it was, sets
  // common file transfer operation parameters to new operation, made
  // new operation current and executes it.
  //

  void executeOperation(FileTransferOperation *newOperation) throw(IOException);
  //
  // Avaliable m_state values
  // FIXME: Maybe make m_state enumeration?
  //

  static const int NOTHING_STATE      = 0x0;
  static const int FILE_LIST_STATE    = 0x1;
  static const int REMOVE_STATE       = 0x2;
  static const int MKDIR_STATE        = 0x3;
  static const int RENAME_STATE       = 0x4;

  static const int LOCAL_REMOVE_STATE = 0x5;

  static const int UPLOAD_STATE       = 0x6;
  static const int DOWNLOAD_STATE     = 0x7;

};

#endif
