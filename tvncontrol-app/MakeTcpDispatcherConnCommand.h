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

#ifndef _MAKE_TCP_DISPATCHER_CONN_COMMAND_H_
#define _MAKE_TCP_DISPATCHER_CONN_COMMAND_H_

#include "util/Command.h"

#include "ControlProxy.h"


// Command that initializes connection to a TcpDispatcher from TightVNC server
// by a specified address.
class MakeTcpDispatcherConnCommand : public Command
{
public:
  MakeTcpDispatcherConnCommand(ControlProxy *serverControl,
                               const TCHAR *connectString,
                               const TCHAR *dispatcherName,
                               const TCHAR *keyword,
                               UINT32 connectionId);
  virtual ~MakeTcpDispatcherConnCommand();

   // Executes command.
   // Inhrited from Command abstract class.
  virtual void execute();

private:
  // Proxy to some of TightVNC server control methods.
  ControlProxy *m_proxy;
  StringStorage m_connectString;
  StringStorage m_dispatcherName;
  StringStorage m_keyword;
  UINT32 m_connectionId;
};

#endif // _MAKE_TCP_DISPATCHER_CONN_COMMAND_H_
