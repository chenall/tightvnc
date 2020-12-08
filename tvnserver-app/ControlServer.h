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

#ifndef _CONTROL_SERVER_H_
#define _CONTROL_SERVER_H_

#include "RfbClientManager.h"
#include "ControlAppAuthenticator.h"
#include "thread/ThreadCollector.h"

#include "network/TcpServer.h"

#include "win-system/PipeServer.h"

/**
 * Tcp server that listens for incoming control connections
 * and give management over these connection to ControlClient instances.
 */
class ControlServer : private Thread
{
public:
  /**
   * Creates and starts control server execution.
   * @param serverTransport ready transport for control server.
   * @param rfbClientManager active TightVNC rfb client manager.
   * @throws Exception when fail to create server.
   * @remark control server takes ownership over server transport.
   * @remark rfbClientManager and rfbServers parameters are needed for
   * executing some of control commands and cannot be 0.
   */
  ControlServer(PipeServer *pipeServer,
                RfbClientManager *rfbClientManager,
                LogWriter *log) throw(Exception);
  /**
   * Stops and deletes control server and deletes transport.
   */
  virtual ~ControlServer();

protected:
  /**
   * Inherited from Thread.
   *
   * Awaits for incoming connections.
   */
  virtual void execute();

  /**
   * Inherited from Thread.
   *
   * Forced closes transport.
   */
  virtual void onTerminate();

private:
  ControlAppAuthenticator m_authenticator;
  ThreadCollector m_threadCollector;

  PipeServer *m_pipeServer;
  /**
   * Active rfb client manager that used in TightVNC server.
   */
  RfbClientManager *m_rfbClientManager;

  LogWriter *m_log;
};

#endif
