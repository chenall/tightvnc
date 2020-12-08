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

#include "HttpServer.h"
#include "HttpClient.h"

#include "thread/ZombieKiller.h"

HttpServer::HttpServer(const TCHAR *bindHost, unsigned short bindPort, bool lockAddr, LogWriter *log)
: TcpServer(bindHost, bindPort, true, lockAddr),
  m_log(log)
{
  m_log->message(_T("Http server started"));
}

HttpServer::~HttpServer()
{
  m_log->message(_T("Http server stopped"));
}

void HttpServer::onAcceptConnection(SocketIPv4 *socket)
{
  TcpClientThread *clientThread = new HttpClient(socket, m_log);

  clientThread->resume();

  ZombieKiller::getInstance()->addZombie(clientThread);
}
