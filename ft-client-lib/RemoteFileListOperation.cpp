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

#include "RemoteFileListOperation.h"

RemoteFileListOperation::RemoteFileListOperation(LogWriter *logWriter,
                                                 const TCHAR *remotePath)
: FileTransferOperation(logWriter),
  m_isOk(false),
  m_isFinished(false)
{
  m_remotePath.setString(remotePath);
}

RemoteFileListOperation::~RemoteFileListOperation()
{
}

void RemoteFileListOperation::start()
{
  m_isOk = false;
  m_isFinished = false;

  m_sender->sendFileListRequest(m_remotePath.getString(),
                                m_replyBuffer->isCompressionSupported());
  notifyStart();
}

void RemoteFileListOperation::onFileListReply(DataInputStream *input)
{
  m_isOk = true;
  m_isFinished = true;
  notifyFinish();
}

void RemoteFileListOperation::onLastRequestFailedReply(DataInputStream *input)
{
  m_isOk = false;
  m_isFinished = true;

  // Logging
  StringStorage message;

  message.format(_T("Error: failed to get file list in remote folder '%s'"),
                 m_remotePath.getString());

  notifyError(message.getString());

  // Notity listeners that operation has finished
  notifyFinish();
}

bool RemoteFileListOperation::isOk()
{
  return m_isFinished && m_isOk;
}
