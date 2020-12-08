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

#include "ConnectionListener.h"
#include "TvnViewer.h"

#include "thread/AutoLock.h"

const TCHAR ConnectionListener::DEFAULT_HOST[] = _T("0.0.0.0");

ConnectionListener::ConnectionListener(WindowsApplication *application,
                                       UINT16 port)
: TcpServer(DEFAULT_HOST, port, true),
  m_application(application)
{
}

ConnectionListener::~ConnectionListener() 
{
  AutoLock al(&m_connectionsLock);
  while (!m_connections.empty()) {
    SocketIPv4 *socket = m_connections.front();
    delete socket;
    m_connections.pop_front();
  }
}

UINT16 ConnectionListener::getBindPort() const
{
  return TcpServer::getBindPort();
}

void ConnectionListener::onAcceptConnection(SocketIPv4 *socket)
{
  AutoLock al(&m_connectionsLock);
  m_connections.push_front(socket);
  m_application->postMessage(TvnViewer::WM_USER_NEW_LISTENING);
}

SocketIPv4 *ConnectionListener::getNewConnection()
{
  AutoLock al(&m_connectionsLock);
  SocketIPv4 *socket = 0;
  if (!m_connections.empty()) {
    socket = m_connections.front();
    m_connections.pop_front();
  }
  return socket;
}
