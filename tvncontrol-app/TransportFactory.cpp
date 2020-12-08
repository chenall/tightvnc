// Copyright (C) 2010,2011,2012 GlavSoft LLC.
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

#include "TransportFactory.h"
#include "SocketIPv4Transport.h"
#include "NamedPipeTransport.h"

#include "win-system/SecurityAttributes.h"

Transport *TransportFactory::createSocketClientTransport(const TCHAR *connectHost,
                                                         unsigned int connectPort)
{
  SocketIPv4 *socket = new SocketIPv4();

  try {
    socket->connect(connectHost, connectPort);
  } catch (SocketException &) {
    delete socket;
    throw;
  }

  return new SocketIPv4Transport(socket);
}

Transport *TransportFactory::createSocketServerTransport(const TCHAR *bindHost,
                                                         unsigned int bindPort)
{
  SocketIPv4 *socket = new SocketIPv4();

  try {
    socket->bind(bindHost, bindPort);
    socket->listen(15);
  } catch (SocketException &) {
    delete socket;
    throw;
  }

  return new SocketIPv4Transport(socket);
}

Transport *TransportFactory::createPipeClientTransport(const TCHAR *name)
{
  return new NamedPipeTransport(PipeClient::connect(name, 0));
}

Transport *TransportFactory::createPipeServerTransport(const TCHAR *name)
{
  // FIXME: Memory leak.
  SecurityAttributes *pipeSecurity = new SecurityAttributes();

  pipeSecurity->setInheritable();
  pipeSecurity->shareToAllUsers();

  return new NamedPipeTransport(new PipeServer(name, 0, pipeSecurity));
}
