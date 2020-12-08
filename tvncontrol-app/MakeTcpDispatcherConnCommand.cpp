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

#include "MakeTcpDispatcherConnCommand.h"

MakeTcpDispatcherConnCommand::MakeTcpDispatcherConnCommand(ControlProxy *serverControl,
                               const TCHAR *connectString,
                               const TCHAR *dispatcherName,
                               const TCHAR *keyword,
                               UINT32 connectionId)
: m_proxy(serverControl),
  m_connectString(connectString),
  m_dispatcherName(dispatcherName),
  m_keyword(keyword),
  m_connectionId(connectionId)
{
}

MakeTcpDispatcherConnCommand::~MakeTcpDispatcherConnCommand()
{
}

void MakeTcpDispatcherConnCommand::execute()
{
  m_proxy->makeTcpDispatcherConnection(m_connectString.getString(),
                                       m_dispatcherName.getString(),
                                       m_keyword.getString(),
                                       m_connectionId);
}
