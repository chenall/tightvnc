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

#ifndef _HTTP_CLIENT_H_
#define _HTTP_CLIENT_H_

#include "network/TcpClientThread.h"

#include "network/socket/SocketIPv4.h"

#include "io-lib/Channel.h"
#include "io-lib/DataInputStream.h"
#include "io-lib/DataOutputStream.h"
#include "log-writer/LogWriter.h"

class HttpClient : public TcpClientThread
{
public:
  HttpClient(SocketIPv4 *socket, LogWriter *log);
  virtual ~HttpClient();

protected:
  virtual void execute();

protected:
  Channel *m_stream;
  // Wrappers around socket stream.
  DataInputStream *m_dIS;
  DataOutputStream *m_dOS;

  LogWriter *m_log;
};

#endif
