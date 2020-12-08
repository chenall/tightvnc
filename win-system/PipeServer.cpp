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

#include "PipeServer.h"
#include "util/Exception.h"

PipeServer::PipeServer(const TCHAR *name, unsigned int bufferSize,
                       SecurityAttributes *secAttr,
                       DWORD milliseconds)
: m_milliseconds(milliseconds),
  m_secAttr(secAttr),
  m_serverPipe(INVALID_HANDLE_VALUE),
  m_bufferSize(bufferSize)
{
  m_pipeName.format(_T("\\\\.\\pipe\\%s"), name);

  createServerPipe();
}

void PipeServer::createServerPipe()
{
  m_serverPipe = CreateNamedPipe(m_pipeName.getString(),   // pipe name
                                 PIPE_ACCESS_DUPLEX |      // read/write access
                                 FILE_FLAG_OVERLAPPED,     // overlapped mode
                                 PIPE_TYPE_BYTE |          // message type pipe
                                 PIPE_READMODE_BYTE |      // message-read mode
                                 PIPE_WAIT,                // blocking mode
                                 PIPE_UNLIMITED_INSTANCES, // max. instances
                                 m_bufferSize,             // output buffer size
                                 m_bufferSize,             // input buffer size
                                 0,                        // client time-out
                                 m_secAttr != 0 ?          // security attributes
                                 m_secAttr->getSecurityAttributes() : 0
                                 );
  if (m_serverPipe == INVALID_HANDLE_VALUE) {
    StringStorage errMess;
    errMess.format(_T("CreateNamedPipe failed, error code = %d"), GetLastError());
    throw Exception(errMess.getString());
  }
}

NamedPipe *PipeServer::accept()
{
  if (m_serverPipe == INVALID_HANDLE_VALUE) {
    createServerPipe();
  }

  OVERLAPPED overlapped;
  memset(&overlapped, 0, sizeof(OVERLAPPED));
  overlapped.hEvent = m_winEvent.getHandle();

  if (ConnectNamedPipe(m_serverPipe, &overlapped)) {
    // In success the overlapped ConnectNamedPipe() function must
    // return zero.
    int errCode = GetLastError();
    StringStorage errMess;
    errMess.format(_T("ConnectNamedPipe failed, error code = %d"), errCode);
    throw Exception(errMess.getString());
  } else {
    int errCode = GetLastError();
    switch(errCode) {
    case ERROR_PIPE_CONNECTED:
      break;
    case ERROR_IO_PENDING:
      m_winEvent.waitForEvent(m_milliseconds);
      DWORD cbRet; // Fake
      if (!GetOverlappedResult(m_serverPipe, &overlapped, &cbRet, FALSE)) {
        int errCode = GetLastError();
        StringStorage errMess;
        errMess.format(_T("GetOverlappedResult() failed after the ")
                       _T("ConnectNamedPipe() call, error code = %d"), errCode);
        throw Exception(errMess.getString());
      }
      break;
    default:
      StringStorage errMess;
      errMess.format(_T("ConnectNamedPipe failed, error code = %d"), errCode);
      throw Exception(errMess.getString());
    }
  }
  // delete is inside ~NamedPipeTransport()
  NamedPipe *result = new NamedPipe(m_serverPipe, m_bufferSize, true);

  m_serverPipe = INVALID_HANDLE_VALUE;

  return result;
}

void PipeServer::close()
{
  /*if (m_isConnected) {
    if (DisconnectNamedPipe(hPipe)) {
      m_isConnected = false;
    } else {
      int errCode = GetLastError();
      StringStorage errMess;
      errMess.format(_T("DisconnectNamedPipe failed, error code = %d"), errCode);
      throw Exception(errMess.getString());
    }
  }*/
  m_winEvent.notify();
}

PipeServer::~PipeServer()
{
  close();
}

void PipeServer::closeConnection()
{
}

void PipeServer::waitForConnect(DWORD milliseconds)
{
}
