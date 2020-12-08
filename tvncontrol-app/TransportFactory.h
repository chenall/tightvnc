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

#ifndef _TRANSPORT_FACTORY_H_
#define _TRANSPORT_FACTORY_H_

#include "Transport.h"

#include "network/socket/SocketIPv4.h"

#include "win-system/Pipe.h"
#include "win-system/PipeClient.h"
#include "win-system/PipeServer.h"

/**
 * Transport factory.
 *
 * Can create various types of transport.
 */
class TransportFactory
{
public:
  /**
   * Creates client transport that uses client socket.
   *
   * Creates socket, connects to host, creates transport using connected socket
   * and returns transport.
   *
   * @param connectHost host to connect for socket.
   * @param connectPort port to connect.
   * @return created transport.
   *
   * @throws SocketException on socket error.
   */
  static Transport *createSocketClientTransport(const TCHAR *connectHost,
                                                unsigned int connectPort) throw(SocketException);
  /**
   * Creates server transport that uses bound socket.
   *
   * Creates socket, bound it, creates transport using socket
   * and returns transport.
   *
   * @param bindHost host to bind.
   * @param bindPort port to bind.
   * @return created transport.
   *
   * @throws SocketException on socket error.
   */
  static Transport *createSocketServerTransport(const TCHAR *bindHost,
                                                unsigned int bindPort) throw(SocketException);

  /**
   * Creates client transport that uses named pipe.
   *
   * @param name name of pipe.
   * @return created transport.
   *
   * @throws Exception on socket error.
   */
  static Transport *createPipeClientTransport(const TCHAR *name) throw(Exception);

  /**
   * Creates server transport that uses named pipe.
   *
   * @param name name of pipe.
   * @return created transport.
   *
   * @throws Exception on socket error.
   */
  static Transport *createPipeServerTransport(const TCHAR *name) throw(Exception);

private:
  /**
   * Don't allow instanizing of factory.
   */
  TransportFactory() { };
};

#endif
