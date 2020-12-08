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

#ifndef _TCP_CONNECTION_H_
#define _TCP_CONNECTION_H_

#include "log-writer/LogWriter.h"
#include "network/RfbInputGate.h"
#include "network/RfbOutputGate.h"
#include "network/socket/SocketIPv4.h"
#include "network/socket/SocketStream.h"
#include "thread/LocalMutex.h"
#include "io-lib/BufferedInputStream.h"

class TcpConnection
{
public:
  TcpConnection(LogWriter *logWriter);
  virtual ~TcpConnection();

  void bind(const TCHAR *host, UINT16 port);
  void bind(SocketIPv4 *socket);
  void bind(RfbInputGate *input, RfbOutputGate *output);

  void connect();
  void close();

  RfbInputGate *getInput() const;
  RfbOutputGate *getOutput() const;
private:
  StringStorage m_host;
  UINT16 m_port;
  SocketIPv4 *m_socket;
  bool m_socketOwner;
  SocketStream *m_socketStream;
  BufferedInputStream *m_bufInput;
  RfbInputGate *m_input;
  RfbOutputGate *m_output;
  bool m_RfbGatesOwner;

  bool m_wasBound;
  bool m_wasConnected;
  bool m_isEstablished;

  LogWriter *m_logWriter;

  mutable LocalMutex m_connectLock;
};

#endif
