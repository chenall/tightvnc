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

#include "RemoteFolderCreateOperation.h"

RemoteFolderCreateOperation::RemoteFolderCreateOperation(LogWriter *logWriter,
                                                         const TCHAR *pathToTargetFile)
: FileTransferOperation(logWriter)
{
  m_pathToTargetFile.setString(pathToTargetFile);
}

RemoteFolderCreateOperation::RemoteFolderCreateOperation(LogWriter *logWriter,
                                                         FileInfo file,
                                                         const TCHAR *pathToTargetRoot)
: FileTransferOperation(logWriter)
{
  FileInfoList *list = new FileInfoList(file);
  getRemotePath(list, pathToTargetRoot, &m_pathToTargetFile);
  // or delete list->getRoot(); ?
  delete list;
}

RemoteFolderCreateOperation::~RemoteFolderCreateOperation()
{
}

void RemoteFolderCreateOperation::start()
{
  // Logging
  StringStorage message;

  message.format(_T("Creating remote folder '%s'"),
                 m_pathToTargetFile.getString());

  notifyInformation(message.getString());

  // Nofity all that operation have started
  notifyStart();

  // Send mkdir request to server
  m_sender->sendMkDirRequest(m_pathToTargetFile.getString());
}

void RemoteFolderCreateOperation::onMkdirReply(DataInputStream *input)
{
  // Notify all that operation have ended
  notifyFinish();
}

void RemoteFolderCreateOperation::onLastRequestFailedReply(DataInputStream *input)
{
  // Logging
  StringStorage message;

  message.format(_T("Error: failed to create remote folder '%s'"),
                 m_pathToTargetFile.getString());

  notifyError(message.getString());

  // Notify all that operation have ended
  notifyFinish();
}
