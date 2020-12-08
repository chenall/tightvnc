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
#include "SocketAddressIPv4.h"
#include "SocketAddressIPv4.h"
#include "SocketIPv4.h"

#include "thread/AutoLock.h"

#include <crtdbg.h>

SocketIPv4::SocketIPv4()
: m_localAddr(NULL), m_peerAddr(NULL), m_isBound(false),
  m_wsaStartup(1, 2)
{
  m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  m_isClosed = false;

  if (m_socket == INVALID_SOCKET) {
    throw SocketException();
  }
}

SocketIPv4::~SocketIPv4()
{
#ifdef _WIN32
  ::closesocket(m_socket);
#else
  ::close(m_socket);
#endif

  AutoLock l(&m_mutex);

  if (m_peerAddr) {
    delete m_peerAddr;
  }

  if (m_localAddr) {
    delete m_localAddr;
  }
}

void SocketIPv4::connect(const TCHAR *host, unsigned short port)
{
  SocketAddressIPv4 address(host, port);

  connect(address);
}

void SocketIPv4::connect(const SocketAddressIPv4 &addr)
{
  struct sockaddr_in targetSockAddr = addr.getSockAddr();

  if (::connect(m_socket, (const sockaddr *)&targetSockAddr, addr.getAddrLen()) == SOCKET_ERROR) {
    throw SocketException();
  }

  AutoLock l(&m_mutex);

  if (m_peerAddr) {
    delete m_peerAddr;
  }

  m_peerAddr = new SocketAddressIPv4(*(struct sockaddr_in *)&targetSockAddr);

  m_isBound = false;
}

int SocketIPv4::available()
{
  int result;
  int err = ::ioctlsocket(m_socket, FIONREAD, reinterpret_cast<u_long*>(&result));
  if (err != 0) 
    return 0;
  return result;
}

void SocketIPv4::close()
{
  m_isClosed = true;
}

void SocketIPv4::shutdown(int how)
{
  if (::shutdown(m_socket, how) == SOCKET_ERROR) {
    throw SocketException();
  }
}

void SocketIPv4::bind(const TCHAR *bindHost, unsigned int bindPort)
{
  SocketAddressIPv4 address(bindHost, bindPort);

  bind(address);
}

void SocketIPv4::bind(const SocketAddressIPv4 &addr)
{
  struct sockaddr_in bindSockaddr = addr.getSockAddr();

  if (::bind(m_socket, (const sockaddr *)&bindSockaddr, addr.getAddrLen()) == SOCKET_ERROR) {
    throw SocketException();
  }

  AutoLock l(&m_mutex);

  if (m_localAddr) {
    delete m_localAddr;
  }

  m_localAddr = new SocketAddressIPv4(*(struct sockaddr_in*)&bindSockaddr);

  m_isBound = true;
}

bool SocketIPv4::isBound()
{
  AutoLock l(&m_mutex);

  return m_isBound;
}

void SocketIPv4::listen(int backlog)
{
  if (::listen(m_socket, backlog) == SOCKET_ERROR) {
    throw SocketException();
  }
}

SocketIPv4 *SocketIPv4::accept()
{
  struct sockaddr_in addr;

  SOCKET result = getAcceptedSocket(&addr);

  SocketIPv4 *accepted;

  try {
    accepted = new SocketIPv4(); 
    accepted->close();
  } catch(...) {
    // Cleanup and throw further
#ifdef _WIN32
    ::closesocket(result);
#else
    ::close(result);
#endif
    throw SocketException();
  }

  // Fall out with exception, no need to check if accepted is NULL
  accepted->set(result);
  return accepted; // Valid and initialized
}

void SocketIPv4::set(SOCKET socket)
{
  AutoLock l(&m_mutex);

#ifdef _WIN32
  ::closesocket(m_socket);
#else
  ::close(m_socket);
#endif
  m_socket = socket;

  // Set local and peer addresses for new socket
  struct sockaddr_in addr;
  socklen_t addrlen = sizeof(struct sockaddr_in);
  if (getsockname(socket, (struct sockaddr *)&addr, &addrlen) == 0) {
    m_localAddr = new SocketAddressIPv4(addr);
  }

  if (getpeername(socket, (struct sockaddr *)&addr, &addrlen) == 0) {
    m_peerAddr = new SocketAddressIPv4(addr);
  }
}

SOCKET SocketIPv4::getAcceptedSocket(struct sockaddr_in *addr)
{
  socklen_t addrlen = sizeof(struct sockaddr_in);
  fd_set afd;

  timeval timeout;
  timeout.tv_sec = 0;
  timeout.tv_usec = 200000;
  SOCKET result = INVALID_SOCKET;

  while (true) {
    FD_ZERO(&afd);
    FD_SET(m_socket, &afd);

    // FIXME: The select() and accept() function can provoke an system
    // exception, if it allows by project settings and closesocket() has alredy
    // been called.
    int ret = select((int)m_socket + 1, &afd, NULL, NULL, &timeout);

    if (m_isClosed || ret == SOCKET_ERROR) {
      throw SocketException();
    } else if (ret == 0) {
      continue;
    } else if (ret > 0) {
      if (FD_ISSET(m_socket, &afd)) {
        result = ::accept(m_socket, (struct sockaddr*)addr, &addrlen);
        if (result == INVALID_SOCKET) {
          throw SocketException();
        }
        break;
      } // if.
    } // if select ret > 0.
  } // while waiting for incoming connection.
  return result;
}

int SocketIPv4::send(const char *data, int size, int flags)
{
  int result;
  
  result = ::send(m_socket, data, size, flags);

  if (result == -1) {
    throw IOException(_T("Failed to send data to socket."));
  }
  
  return result;
}

int SocketIPv4::recv(char *buffer, int size, int flags)
{
  int result;

  result = ::recv(m_socket, buffer, size, flags);

  // Connection has been gracefully closed.
  if (result == 0) {
    throw IOException(_T("Connection has been gracefully closed"));
  }

  // SocketIPv4 error.
  if (result == SOCKET_ERROR) {
    throw IOException(_T("Failed to recv data from socket."));
  }

  return result;
}

bool SocketIPv4::getLocalAddr(SocketAddressIPv4 *addr)
{
  AutoLock l(&m_mutex);

  if (m_localAddr == 0) {
    return false;
  }

  *addr = *m_localAddr;

  return true;
}

bool SocketIPv4::getPeerAddr(SocketAddressIPv4 *addr)
{
  AutoLock l(&m_mutex);

  if (m_peerAddr == 0) {
    return false;
  }

  *addr = *m_peerAddr;

  return true;
}

/* Auxiliary */
void SocketIPv4::setSocketOptions(int level, int name, void *value, socklen_t len)
{
  if (setsockopt(m_socket, level, name, (char*)value, len) == SOCKET_ERROR) {
    throw SocketException();
  }
}

void SocketIPv4::getSocketOptions(int level, int name, void *value, socklen_t *len)
{
  if (getsockopt(m_socket, level, name, (char*)value, len) == SOCKET_ERROR) {
    throw SocketException();
  }
}

void SocketIPv4::enableNaggleAlgorithm(bool enabled)
{
  BOOL disabled = enabled ? 0 : 1;

  setSocketOptions(IPPROTO_TCP, TCP_NODELAY, &disabled, sizeof(disabled));
}

void SocketIPv4::setExclusiveAddrUse()
{
  int val = 1;

  setSocketOptions(SOL_SOCKET, SO_EXCLUSIVEADDRUSE, &val, sizeof(val));
}
