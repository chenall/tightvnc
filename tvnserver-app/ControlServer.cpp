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

#include "ControlServer.h"
#include "ControlClient.h"
#include "tvncontrol-app/NamedPipeTransport.h"

ControlServer::ControlServer(PipeServer *pipeServer,
                             RfbClientManager *rfbClientManager,
                             LogWriter *log)
: m_authenticator(30000, 3),
  m_pipeServer(pipeServer),
  m_rfbClientManager(rfbClientManager),
  m_log(log)
{
  m_log->message(_T("%s"), _T("Control server started"));

  resume();
}

ControlServer::~ControlServer()
{
  m_log->message(_T("Destroying control server transport"));

  terminate();
  wait();

  try {
    m_pipeServer->close();
  } catch (Exception &ex) {
    m_log->error(_T("Failed to destroy control server transport with '%s' reason"), ex.getMessage());
  }

  delete m_pipeServer;

  // Unblock all client if it has been blocked by authenticator
  m_authenticator.breakAndDisableAuthentications();

  m_log->message(_T("%s"), _T("Control server stopped"));
}

void ControlServer::execute()
{
  try {
    while (!isTerminating()) {
      NamedPipe *pipe = m_pipeServer->accept();
      Transport *transport = new NamedPipeTransport(pipe);

      ControlClient *clientThread = new ControlClient(transport,
                                                      m_rfbClientManager,
                                                      &m_authenticator,
                                                      pipe->getHandle(),
                                                      m_log);

      clientThread->resume();

      m_threadCollector.addThread(clientThread);
    }
  } catch (Exception &ex) {
    m_log->error(_T("Exception on control server thread: %s"), ex.getMessage());
  }
}

void ControlServer::onTerminate()
{
  try { m_pipeServer->close(); } catch (...) { }
}
