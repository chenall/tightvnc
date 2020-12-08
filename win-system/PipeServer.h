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

#ifndef __PIPESERVER_H__
#define __PIPESERVER_H__

#include "util/CommonHeader.h"

#include "NamedPipe.h"
#include "thread/LocalMutex.h"
#include "SecurityAttributes.h"

/**
 * Server fabric of pipes.
 *
 * @author yuri, enikey.
 * @fixme not thread-safe, undone, strange code inside.
 */
class PipeServer
{
public:
  /**
   * Creates new pipe server ready to accept connections.
   *
   * @param pipeName name of pipe to bind.
   * @throws PipeException on fail.
   *
   * @fixme stub.
   */
  PipeServer(const TCHAR *name, unsigned int bufferSize,
             SecurityAttributes *secAttr = 0,
             DWORD milliseconds = INFINITE) throw(Exception);
  /**
   * Destroys pipe server.
   */
  virtual ~PipeServer();

  /**
   * Waits until pipe client connects.
   * @return connected pipe.
   * @throws PipeException on fail.
   * @note accept() method can be breaked by call of close() method.
   *
   * @fixme stub.
   */
  NamedPipe *accept() throw(Exception);

  /**
   * Closes pipe server.
   * @throws PipeException on fail.
   */
  void close() throw(Exception);

  /**
   * @deprecated.
   */
  void waitForConnect(DWORD milliseconds = INFINITE);
  /**
   * @deprecated.
   */
  virtual void closeConnection();

private:
  void createServerPipe() throw(Exception);
private:
  StringStorage m_pipeName;
  WindowsEvent m_winEvent;
  DWORD m_milliseconds;
  SecurityAttributes *m_secAttr;
  HANDLE m_serverPipe;
  unsigned int m_bufferSize;
};

#endif // __PIPESERVER_H__
