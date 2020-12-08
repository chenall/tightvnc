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

#ifndef _UPLOAD_OPERATION_H_
#define _UPLOAD_OPERATION_H_

#include "file-lib/File.h"
#include "file-lib/WinFileChannel.h"
#include "FileTransferOperation.h"
#include "FileInfoList.h"
#include "CopyOperation.h"

//
// File transfer operation class for uploading files (and file trees).
//
//
// Remark: Class uses difficult trick to get remote directory file list
// (to check file collisions, when we trying to upload some file,
// but it's already exist on remote machine). 
//
// So, we need to send file list request every time before step one folder down
// or up (as you know, all this features realized in 'gotoNext' method) and when reply
// for this request (FLR) is received, we must execute old piece of code from
// 'gotoNext' method to really step one folder down or up.
//
// Also we must do this trick in 'start' method, cause we cannot start upload
// immedianly. We can start uploading files only when we will know remote
// destination folder's filelist.
//

class UploadOperation : public CopyOperation
{
public:

  //
  // Arguments:
  //
  // fileToUpload - information about file to upload, filename in this class
  // must be relative (only filename, not full path to file).
  //
  // pathToSourceRoot - absolute path on local file system to folder where files to upload
  // is located (system depended).
  //
  // pathToTargetRoot - absolute path to destination directory where uploaded file tree
  // must be located on remote file system after upload (system independed, in file transfer
  // filename format, see file transfer protocol extension for more information).
  //

  UploadOperation(LogWriter *logWriter,
                  FileInfo fileToUpload,
                  const TCHAR *pathToSourceRoot,
                  const TCHAR *pathToTargetRoot);

  UploadOperation(LogWriter *logWriter,
                  const FileInfo *filesToUpload,
                  size_t filesCount,
                  const TCHAR *pathToSourceRoot,
                  const TCHAR *pathToTargetRoot);

  virtual ~UploadOperation();

  //
  // Starts upload operation
  //

  virtual void start() throw(IOException);

  //
  // Inherited from FileTransferEventHandler class
  //
  // Event handlers to process file transfer messages
  //

  virtual void onUploadReply(DataInputStream *input) throw(IOException);
  virtual void onUploadDataReply(DataInputStream *input) throw(IOException);
  virtual void onUploadEndReply(DataInputStream *input) throw(IOException);
  virtual void onMkdirReply(DataInputStream *input) throw(IOException);
  virtual void onLastRequestFailedReply(DataInputStream *input) throw(IOException);
  virtual void onFileListReply(DataInputStream *input) throw(IOException);

private:

  UINT64 getInputFilesSize();
  UINT64 getFileSize(const TCHAR *pathToFile);

protected:

  // Terminates operation execution
  void killOp();

  //
  // Special ft message handler for "Last request failed" and "File list request".
  //

  bool specialHandler() throw(IOException);

  //
  // Start current file upload (current file is notated by m_toCopy value).
  //

  void startUpload() throw(IOException);

  //
  // Lists folder from local file system and set this list as child to current
  // file list (m_toCopy), after than it tryies to create folder on remote
  // file system.
  //

  void processFolder() throw(IOException);  

  //
  // Tryies to start upload of current file (notated by m_toCopy member).
  //

  void processFile() throw(IOException);

  //
  // Calls gotoNext method with true 'fake' argument
  //

  void gotoNext();

  //
  // Choose next file to upload from m_toCopy list to m_toCopy list.
  //
  // Remark: Also made some magic things with m_toCopy list to avoid "infinity" loops.
  // Remark: if fake is true (it's true by default) method does not change current file
  // to child or parent, and sets 'Get file list trick' flags and sends file list request.
  //

  void gotoNext(bool fake) throw(IOException);

  //
  // Reads data chunk from current uploading file and
  // sends it to server.
  //

  void sendFileDataChunk() throw(IOException);

  //
  // Helper methods to control m_remoteFilesInfo, m_remoteFilesCount
  // members.
  //

  void releaseRemoteFilesInfo();
  void initRemoteFiles(FileInfo *remoteFilesInfo, UINT32 count);

  // Helper method that creates message and notify listeners
  void notifyFailedToUpload(const TCHAR *errorDescription);

  // Changed m_toCopy member and updates m_pathToSourceFile,
  // m_pathToTargetFile members
  void changeFileToUpload(FileInfoList *toUpload);

protected:
  // Source file that we uploading now
  File *m_file;
  // File input stream associated with m_file
  WinFileChannel *m_fis;

  // File list of remote directory where we uploading
  // current file now
  FileInfo *m_remoteFilesInfo;
  UINT32 m_remoteFilesCount;

  //
  // Helper members of 'Remote file list trick'
  //

  bool m_gotoChild;
  bool m_gotoParent;
  bool m_firstUpload;
};

#endif
