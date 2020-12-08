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

#include "OutgoingRfbConnectionThread.h"

OutgoingRfbConnectionThread::OutgoingRfbConnectionThread(const TCHAR *connectHost,
                                                         unsigned int connectPort,
                                                         bool viewOnly,
                                                         RfbClientManager *clientManager,
                                                         LogWriter *log)
: m_connectHost(connectHost), m_connectPort(connectPort), m_viewOnly(viewOnly),
  m_clientManager(clientManager),
  m_log(log)
{
}

OutgoingRfbConnectionThread::~OutgoingRfbConnectionThread()
{
  
}

void OutgoingRfbConnectionThread::execute()
{
  SocketIPv4 *socket = new SocketIPv4();

  try {
    socket->connect(m_connectHost.getString(), m_connectPort);
  } catch (Exception &someEx) {
    m_log->error(_T("Failed to connect to %s:%d with reason: '%s'"),
               m_connectHost.getString(), m_connectPort, someEx.getMessage());
    delete socket;
    return ;
  }

  m_clientManager->addNewConnection(socket,
                                    &ViewPortState(), // with a default view port
                                    m_viewOnly, true);
}
