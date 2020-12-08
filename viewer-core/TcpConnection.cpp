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

#include "thread/AutoLock.h"

#include "TcpConnection.h"

TcpConnection::TcpConnection(LogWriter *logWriter)
: m_logWriter(logWriter),
m_socketOwner(false),
m_RfbGatesOwner(false)
{
  m_port = 0;
  m_socket = 0;
  m_socketStream = 0;
  m_input = 0;
  m_output = 0;
  m_wasBound = false;
  m_wasConnected = false;
  m_isEstablished = false;
}

void TcpConnection::bind(const TCHAR *host, UINT16 port)
{
  AutoLock al(&m_connectLock);
  if (m_wasBound) {
    throw Exception(_T("Tcp-connection already bound"));
  }
  m_host = host;
  m_port = port;
  m_socket = 0;
  m_input = 0;
  m_output = 0;
  m_wasBound = true;
}

void TcpConnection::bind(SocketIPv4 *socket)
{
  AutoLock al(&m_connectLock);
  if (m_wasBound) {
    throw Exception(_T("Tcp-connection already bound"));
  }
  m_host = _T("");
  m_port = 0;
  m_socket = socket;
  m_input = 0;
  m_output = 0;
  m_wasBound = true;
}

void TcpConnection::bind(RfbInputGate *input, RfbOutputGate *output)
{
  AutoLock al(&m_connectLock);
  if (m_wasBound) {
    throw Exception(_T("Tcp-connection already bound"));
  }
  m_host = _T("");
  m_port = 0;
  m_socket = 0;
  m_input = input;
  m_output = output;
  m_wasBound = true;
}

void TcpConnection::connect()
{
  // if connection is already established, then method do nothing.
  {
    AutoLock al(&m_connectLock);
    m_wasConnected = true;
  }
  // need create to gates
  if (m_input == 0 && m_output == 0) {
    // need create to socket
    if (m_socket == 0) {
      if (!m_host.isEmpty() && m_port != 0) {
        SocketAddressIPv4 ipAddress(m_host.getString(), m_port);

        StringStorage ipAddressString;
        ipAddress.toString(&ipAddressString);
        m_logWriter->detail(_T("Connecting to the host \"%s:%hd\" (%s:%hd)..."),
                            m_host.getString(), m_port,
                            ipAddressString.getString(), m_port);

        m_socket = new SocketIPv4;
        m_socketOwner = true;
        m_socket->connect(ipAddress);
        m_socket->enableNaggleAlgorithm(false);
      } else {
        throw Exception(_T("Connection parameters (host, port, socket, gates) is empty."));
      }
    }

    m_logWriter->detail(_T("Initialization of socket stream and input/output gates..."));
    m_socketStream = new SocketStream(m_socket);
    m_bufInput = new BufferedInputStream(m_socketStream);
    m_input = new RfbInputGate(m_bufInput);
    m_output = new RfbOutputGate(m_socketStream);
    m_RfbGatesOwner = true;
  } else {
    _ASSERT(m_input != 0 && m_output != 0);
  }
  {
    AutoLock al(&m_connectLock);
    m_isEstablished = true;
  }
}

void TcpConnection::close()
{
  if (!m_host.isEmpty() && m_port != 0) {
    if (m_socketStream != 0) {
      m_socketStream->close();
    }
  }
}

RfbInputGate *TcpConnection::getInput() const
{
  {
    AutoLock al(&m_connectLock);
    if (!m_isEstablished) {
      throw Exception(_T("Connecting has not been established"));
    }
  }
  return m_input;
}

RfbOutputGate *TcpConnection::getOutput() const
{
  {
    AutoLock al(&m_connectLock);
    if (!m_isEstablished) {
      throw Exception(_T("Connection has not been established"));
    }
  }
  return m_output;
}

TcpConnection::~TcpConnection()
{
  // if socket is defined, then need delete gates and socket stream
  if (m_socket != 0) {
    if (m_input != 0 && m_RfbGatesOwner) {
      try {
        delete m_input;
      } catch (...) {
      }
    }

    if (m_output != 0 && m_RfbGatesOwner) {
      try {
        delete m_output;
      } catch (...) {
      }
    }

    if (m_socketStream != 0) {
      try {
        delete m_socketStream;
      }
      catch (...) {
      }
    }
    if (m_bufInput != 0) {
      try {
        delete m_bufInput;
      }
      catch (...) {
      }
    }

  }

  // if host and port is defined, then need delete socket
  try {
    if (m_socket != NULL && !m_host.isEmpty() && m_port && m_socketOwner) {
      delete m_socket;
      m_socket = NULL;
    }
  } catch (...) {
  }
}
