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

#include "ListenLogServer.h"

ListenLogServer::ListenLogServer(const TCHAR *publicPipeName,
                                 ConnectionListener *connListener)
: m_connListener(connListener)
{
  m_pipeSecurity.shareToAllUsers();
  const unsigned int logBufferSizeForServiceChannel = 0x10000;
  m_listenPipe = new PipeServer(publicPipeName, logBufferSizeForServiceChannel);
  resume();
}

ListenLogServer::~ListenLogServer()
{
  terminate();
  m_timer.notify();
  m_listenPipe->close();
  wait();
  delete m_listenPipe;
}

void ListenLogServer::execute()
{
  while (!isTerminating()) {
    try {
      NamedPipe *pipe = m_listenPipe->accept();
      m_connListener->onNewConnection(pipe);
    } catch (...) {
      if (!isTerminating()) {
        m_timer.waitForEvent(1000); // Pause
      }
    }
  }
}
