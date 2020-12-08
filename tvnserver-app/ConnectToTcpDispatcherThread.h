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

#ifndef _CONNECTTOTCPDISPATCHERTHREAD_H_
#define _CONNECTTOTCPDISPATCHERTHREAD_H_

#include "util/StringStorage.h"
#include "thread/Thread.h"
#include "RfbClientManager.h"
#include "util/AnsiStringStorage.h"
#include "log-writer/LogWriter.h"

/**
Thread that makes attempt to connect to listening rfb client in separate thread.
To use it, you must create instance, call start method, and pass it to zombie killer.
@remark don't forget to add it to zombie killer after thread is started.
@author enikey.
*/
class ConnectToTcpDispatcherThread : public Thread
{
public:
  /**
  Creates new instance of outgoing rfb connection thread.
  @param connectHost target host to connect.
  @param connectPort port to connect.
  @param viewOnly flag that determinates mode for rfb connection.
  @param clientManager rfb client manager.
  Throws Exception on an error.
  */
  ConnectToTcpDispatcherThread(const TCHAR *connectHost, unsigned int connectPort,
                               const AnsiStringStorage *dispatcherName,
                               UINT32 connectionId,
                               const AnsiStringStorage *keyword,
                               RfbClientManager *clientManager,
                               LogWriter *log);
  virtual ~ConnectToTcpDispatcherThread();

protected:
  virtual void execute();
  virtual void onTerminate();

private:
  StringStorage m_connectHost;
  unsigned int m_connectPort;
  AnsiStringStorage m_dispatcherName;
  UINT32 m_connectionId;
  AnsiStringStorage m_keyword;

  // FIXME: Make sure it always points to a live object.
  RfbClientManager *m_clientManager;

  // This class owns the socket until the socket will be passed to another
  // object. Socket delegation must be protected by a mutex to make sure we
  // know who is the owner at the moment of closing the socket.
  SocketIPv4 *m_socket;
  bool m_pendingToRemove;
  LocalMutex m_socketDelegationMutex;

  // FIXME: Make sure it always points to a live object.
  LogWriter *m_log;
};

#endif // _CONNECTTOTCPDISPATCHERTHREAD_H_
