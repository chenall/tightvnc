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

#include "TcpServer.h"

#include "network/socket/SocketAddressIPv4.h"

TcpServer::TcpServer(const TCHAR *bindHost, unsigned short bindPort,
                     bool autoStart,
                     bool lockAddr)
: m_bindHost(bindHost), m_bindPort(bindPort)
{
  SocketAddressIPv4 bindAddr = SocketAddressIPv4::resolve(bindHost, bindPort);

  if (lockAddr) {
    m_listenSocket.setExclusiveAddrUse();
  }

  m_listenSocket.bind(bindAddr);
  m_listenSocket.listen(10);

  if (autoStart) {
    start();
  }
}

TcpServer::~TcpServer()
{
  try { m_listenSocket.shutdown(SD_BOTH); } catch(...) { }
  try { m_listenSocket.close(); } catch (...) { }

  if (isActive()) {
    Thread::terminate();
    Thread::wait();
  }
}

const TCHAR *TcpServer::getBindHost() const
{
  return m_bindHost.getString();
}

unsigned short TcpServer::getBindPort() const
{
  return m_bindPort;
}

void TcpServer::start()
{
  resume();
}

void TcpServer::execute()
{
  while (!isTerminating()) {
    SocketIPv4 *clientSocket = NULL;

    try {
      clientSocket = m_listenSocket.accept();
    } catch (...) {
      clientSocket = NULL;
    }

    if (clientSocket != NULL) {
      onAcceptConnection(clientSocket);
    } else {
      break ;
    }
  }
}
