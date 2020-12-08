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

#ifndef SOCKET_IPV4_H
#define SOCKET_IPV4_H

#include "sockdefs.h"

#include "SocketIPv4.h"
#include "SocketAddressIPv4.h"
#include "SocketException.h"

#include "io-lib/Channel.h"
#include "io-lib/IOException.h"
#include "win-system/WsaStartup.h"
#include "thread/LocalMutex.h"

/**
 * IPv4 Socket class.
 *
 * @fixme refactor this class to avoid usage of SocketAddressIPv4 class.
 */
class SocketIPv4
{
public:
  /**
   * Creates new socket.
   */
  SocketIPv4();
  /**
   * Deletes and closes socket.
   */
  virtual ~SocketIPv4();

  /**
   * Connects to remote host.
   * @param host host to connect.
   * @param port port to connect.
   * @throws SocketException on fail.
   */
  void connect(const TCHAR *host, unsigned short port) throw(SocketException);
  /**
   * Connects to remote host.
   * @param addr address to connect.
   * @throws SocketException on fail.
   */
  void connect(const SocketAddressIPv4 &addr) throw(SocketException);
  /**
   * Closes socket.
   * @throws SocketException on fail.
   */
  void close() throw(SocketException);
  /**
   * Shutdowns socket.
   * @param how how to shutdown socket (SD_RECEIVE|SD_SEND|SD_BOTH).
   * @throws SocketException on fail.
   */
  void shutdown(int how) throw(SocketException);
  /**
   * Binds socket to specified address.
   * @param bindHost host to bind.
   * @param bindPort port to bind.
   * @throws SocketException on fail.
   */
  void bind(const TCHAR *bindHost, unsigned int bindPort);
  /**
   * Binds socket to socket address.
   * @throws SocketException on fail.
   */
  void bind(const SocketAddressIPv4 &addr) throw(SocketException);
  /**
   * Checks if this socket is bound and used for server needs.
   * @return true if socket used as server and successfully bounded, false otherwise.
   */
  bool isBound();
  /**
   * Socket listen method.
   * @param backlog max count of connections in pool.
   * @throws SocketException on fail.
   */
  void listen(int backlog) throw(SocketException);

  /**
   * Accepts incoming connection.
   * @throws SocketException on fail.
   * @return newly allocated socket that contain incoming connections.
   */
  SocketIPv4 *accept() throw(SocketException);

  /**
   * Sends data to socket.
   *
   * @param data buffer to send.
   * @param size bytes to send.
   * @param [optional] flags socket flags.
   * @return count to sent bytes.
   * @throw IOException on error.
   */
  int send(const char *data, int size, int flags = 0) throw(IOException);
  /**
   * Receives data from socket.
   *
   * @param buffer buffer to receive data.
   * @param size count of bytes to read from socket.
   * @param flags recv flags.
   * @return count of read bytes.
   * @throws IOException on fail.
   */
  int recv(char *buffer, int size, int flags = 0) throw(IOException);

  int available();

  /**
   * Returns local address of socket (for listening socket).
   * @param addr output parameter that will contain socket address.
   * @return true on success, false on fail.
   */
  bool getLocalAddr(SocketAddressIPv4 *addr);
  /**
   * Returns peer address.
   * @param addr output parameter that will contain socket address.
   * @return true on success, false on fail.
   */
  bool getPeerAddr(SocketAddressIPv4 *addr);

  /* Auxiliary */
  void setSocketOptions(int level, int name, void *value, socklen_t len) throw(SocketException);
  void getSocketOptions(int level, int name, void *value, socklen_t *len) throw(SocketException);

  /* Socket options */
  void enableNaggleAlgorithm(bool enabled) throw(SocketException);
  void setExclusiveAddrUse() throw(SocketException);

private:
  WsaStartup m_wsaStartup;

protected:
  // Returns a SOCKET object with performed accept operation.
  // Throws SocketException on an error.
  SOCKET getAcceptedSocket(struct sockaddr_in *addr);

  // Closes old socket and sets handler to new one
  void set(SOCKET socket);

  /**
   * Mutex for thread-safety.
   */
  LocalMutex m_mutex;

  /**
   * WinSock socket.
   */
  SOCKET m_socket;
  bool m_isClosed;

  SocketAddressIPv4 *m_localAddr;
  SocketAddressIPv4 *m_peerAddr;

  /**
   * Flag determinating if socket is server or client socket.
   */
  bool m_isBound;
};

#endif
