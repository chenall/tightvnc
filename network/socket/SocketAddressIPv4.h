// Copyright (C) 2008,2009,2010,2011,2012 GlavSoft LLC.
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

#ifndef SOCKET_ADDRESS_IPV4_H
#define SOCKET_ADDRESS_IPV4_H

#include "util/CommonHeader.h"
#include "thread/LocalMutex.h"
#include "SocketException.h"
#include "sockdefs.h"
#include "win-system/WsaStartup.h"

// FIXME: Deprecated method, only for testing of old code.
void getLocalIPAddrString(char *buffer, int buflen);

class SocketAddressIPv4
{
public:
  SocketAddressIPv4();
  SocketAddressIPv4(struct sockaddr_in);
  SocketAddressIPv4(const TCHAR *host, unsigned short port);

  SocketAddressIPv4(const SocketAddressIPv4 &socketAddressIPv4);
  SocketAddressIPv4 &operator=(const SocketAddressIPv4 &socketAddressIPv4);

  socklen_t getAddrLen() const;
  struct sockaddr_in getSockAddr() const;

  // Converts socket address to it's string value (ip address as string).
  void toString(StringStorage *address) const;

  // Returns socket port number 
  unsigned short SocketAddressIPv4::getPort() const;

  static SocketAddressIPv4 resolve(const TCHAR *host, unsigned short port) throw(SocketException);

protected:
  WsaStartup m_wsaStartup;
  unsigned short m_port;
  struct in_addr m_addr;

  static LocalMutex s_resolveMutex;
};

#endif
