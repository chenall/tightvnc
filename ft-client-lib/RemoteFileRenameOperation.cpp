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

#include "RemoteFileRenameOperation.h"

RemoteFileRenameOperation::RemoteFileRenameOperation(LogWriter *logWriter,
                                                     const TCHAR *pathToSourceFile,
                                                     const TCHAR *pathToTargetFile)
: FileTransferOperation(logWriter)
{
  m_pathToSourceFile.setString(pathToSourceFile);
  m_pathToTargetFile.setString(pathToTargetFile);
}

RemoteFileRenameOperation::RemoteFileRenameOperation(LogWriter *logWriter,
                                                     FileInfo sourceFileInfo,
                                                     FileInfo targetFileInfo,
                                                     const TCHAR *pathToTargetRoot)
: FileTransferOperation(logWriter)
{
  FileInfoList srcList(sourceFileInfo);
  FileInfoList dstList(targetFileInfo);

  getRemotePath(&srcList, pathToTargetRoot, &m_pathToSourceFile);
  getRemotePath(&dstList, pathToTargetRoot, &m_pathToTargetFile);
}

RemoteFileRenameOperation::~RemoteFileRenameOperation()
{
}

void RemoteFileRenameOperation::start()
{
  // Logging
  StringStorage message;

  message.format(_T("Renaming remote file '%s' to '%s'"),
                 m_pathToSourceFile.getString(),
                 m_pathToTargetFile.getString());

  notifyInformation(message.getString());

  // Notify all that operation have started
  notifyStart();

  // Send request to server
  m_sender->sendMvFileRequest(m_pathToSourceFile.getString(),
                              m_pathToTargetFile.getString());
}

void RemoteFileRenameOperation::onMvReply(DataInputStream *input)
{
  // Notify listeners that operation has finished
  notifyFinish();
}

void RemoteFileRenameOperation::onLastRequestFailedReply(DataInputStream *input)
{
  // Logging
  StringStorage message;

  message.format(_T("Error: failed to rename remote '%s' file"),
                 m_pathToSourceFile.getString());

  notifyInformation(message.getString());

  // Notify listeners that operation has finished
  notifyFinish();
}
