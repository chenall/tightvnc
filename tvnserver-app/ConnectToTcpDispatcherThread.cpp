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

#include "ConnectToTcpDispatcherThread.h"
#include "network/socket/SocketStream.h"
#include "io-lib/DataOutputStream.h"
#include "io-lib/DataInputStream.h"
#include "tcp-dispatcher/TcpDispatcherInitializer.h"

ConnectToTcpDispatcherThread::ConnectToTcpDispatcherThread(
  const TCHAR *connectHost,
  unsigned int connectPort,
  const AnsiStringStorage *dispatcherName,
  UINT32 connectionId,
  const AnsiStringStorage *keyword,
  RfbClientManager *clientManager,
  LogWriter *log)

: m_connectHost(connectHost),
  m_connectPort(connectPort),
  m_dispatcherName(*dispatcherName),
  m_connectionId(connectionId),
  m_keyword(*keyword),
  m_clientManager(clientManager),
  m_socket(new SocketIPv4()),
  m_pendingToRemove(false),
  m_log(log)
{
  resume();
}

ConnectToTcpDispatcherThread::~ConnectToTcpDispatcherThread()
{
  terminate();
  wait();
  if (m_socket != 0) {
    delete m_socket; // We are still the socket owner
  }
}

void ConnectToTcpDispatcherThread::onTerminate()
{
  AutoLock al(&m_socketDelegationMutex);
  m_pendingToRemove = true;
  if (m_socket != 0) {
    // Interrupt blocking operations on the socket.
    try { m_socket->shutdown(SD_BOTH); } catch (...) { }
    try { m_socket->close(); } catch (...) { }
  }
}

void ConnectToTcpDispatcherThread::execute()
{
  try {
    m_socket->connect(m_connectHost.getString(), m_connectPort);
    SocketStream sockStream(m_socket);

    TcpDispatcherInitializer tcpDisp(&sockStream,
                                     &m_dispatcherName,
                                     0,
                                     m_connectionId,
                                     &m_keyword, m_log);
    tcpDisp.readProtocolType();

    tcpDisp.continueTcpDispatchProtocol();

    // // NOTE: Currently, we don't use the received Dispatcher name.
    // AnsiStringStorage gotDispatcherName;
    // tcpDisp.getRecivedDispatcherName(&gotDispatcherName);

    tcpDisp.waitNextProtocolContinue();

    {
      AutoLock al(&m_socketDelegationMutex);
      if (!m_pendingToRemove) {
        m_clientManager->addNewConnection(m_socket,
                                          &ViewPortState(), // with a default view port
                                          false, false);
        m_socket = 0; // Now, we aren't the socket owner.
      }
    }
  } catch (Exception &someEx) {
    m_log->error(_T("Failed to connect to %s:%d with reason: '%s'"),
               m_connectHost.getString(), m_connectPort, someEx.getMessage());

    // FIXME: Check what will happen on catching exception here.
    //        Should we reset m_socket to 0? Should we close it?
  }
}
