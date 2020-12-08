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

#include <stdlib.h>
#include <string.h>
#include <vector>

#include "sockdefs.h"
#include "SocketAddressIPv4.h"
#include "SocketAddressIPv4.h"
#include "SocketException.h"
#include "util/AnsiStringStorage.h"

#include "thread/AutoLock.h"

LocalMutex SocketAddressIPv4::s_resolveMutex;

void getLocalIPAddrString(char *buffer, int buflen)
{
  char namebuf[256];

  if (gethostname(namebuf, 256) != 0) {
    strncpy(buffer, "Host name unavailable", buflen);
    return;
  };

  HOSTENT *ph = gethostbyname(namebuf);
  if (!ph) {
    strncpy(buffer, "IP address unavailable", buflen);
    return;
  };

  *buffer = '\0';
  char digtxt[5];
  for (int i = 0; ph->h_addr_list[i]; i++) {
    for (int j = 0; j < ph->h_length; j++) {
      sprintf(digtxt, "%d.", (unsigned char) ph->h_addr_list[i][j]);
      strncat(buffer, digtxt, (buflen-1)-strlen(buffer));
    }
  buffer[strlen(buffer)-1] = '\0';
  if (ph->h_addr_list[i+1] != 0)
    strncat(buffer, ", ", (buflen-1)-strlen(buffer));
  }
}

SocketAddressIPv4::SocketAddressIPv4()
: m_wsaStartup(1, 2)
{
  m_addr.s_addr = INADDR_ANY;
  m_port = 0;
};

SocketAddressIPv4::SocketAddressIPv4(struct sockaddr_in addr)
: m_wsaStartup(1, 2)
{
  m_addr.s_addr = INADDR_ANY;
  m_port = 0;

  if (addr.sin_family != AF_INET) {
    throw SocketException(_T("The specified m_addr is not AF_INET family m_addr!"));
  }

  m_addr.s_addr = ntohl(addr.sin_addr.s_addr);
  m_port = ntohs(addr.sin_port);
};

SocketAddressIPv4::SocketAddressIPv4(const TCHAR *host, unsigned short port)
: m_wsaStartup(1, 2)
{
  SocketAddressIPv4 sa = SocketAddressIPv4::resolve(host, port);

  this->m_addr = sa.m_addr;
  this->m_port = sa.m_port;
};

SocketAddressIPv4::SocketAddressIPv4(const SocketAddressIPv4 &socketAddressIPv4)
: m_wsaStartup(1, 2) {
  m_addr = socketAddressIPv4.m_addr;
  m_port = socketAddressIPv4.m_port;
}

SocketAddressIPv4& SocketAddressIPv4::operator=(const SocketAddressIPv4 &socketAddressIPv4) {
  if (this != &socketAddressIPv4) {
    this->m_addr = socketAddressIPv4.m_addr;
    this->m_port = socketAddressIPv4.m_port;
  }
  return *this;
}

socklen_t SocketAddressIPv4::getAddrLen() const
{
  return sizeof(struct sockaddr_in);
};

struct sockaddr_in SocketAddressIPv4::getSockAddr() const
{
  struct sockaddr_in saddr;

  saddr.sin_family = AF_INET;
  saddr.sin_addr.s_addr = htonl(m_addr.s_addr);
  saddr.sin_port = htons(m_port);

#ifndef _WIN32
  saddr.sin_len = sizeof(struct sockaddr_in);
#endif

  return saddr;
}

void SocketAddressIPv4::toString(StringStorage *address) const
{
  u_char b1 = m_addr.S_un.S_un_b.s_b4;
  u_char b2 = m_addr.S_un.S_un_b.s_b3;
  u_char b3 = m_addr.S_un.S_un_b.s_b2;
  u_char b4 = m_addr.S_un.S_un_b.s_b1;

  address->format(_T("%d.%d.%d.%d"), b1, b2, b3, b4);
}

unsigned short SocketAddressIPv4::getPort() const
{
  return m_port;
}

SocketAddressIPv4 SocketAddressIPv4::resolve(const TCHAR *host, unsigned short m_port)
{
  SocketAddressIPv4 resolvedAddress;

  StringStorage hostStorage(host);

  {
    AutoLock l(&s_resolveMutex);

    AnsiStringStorage hostAnsi(&hostStorage);

    hostent *hent = gethostbyname(hostAnsi.getString());
    if (hent == 0) {
      throw SocketException();
    }

    resolvedAddress.m_addr.S_un.S_addr = ntohl(*(u_long *)hent->h_addr_list[0]);
  }

  resolvedAddress.m_port = m_port;

  return resolvedAddress;
}
