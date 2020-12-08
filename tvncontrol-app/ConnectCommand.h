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

#ifndef __CONNECTCOMMAND_H__
#define __CONNECTCOMMAND_H__

#include "util/Command.h"

#include "ControlProxy.h"

/**
 * Command that invokes remote disconnectAll() method of TightVNC server
 * using control transport.
 */
class ConnectCommand : public Command
{
public:
  /**
   * Creates command.
   * @param serverControl proxy.
   */
  ConnectCommand(ControlProxy *serverControl, const TCHAR *hostName);
  /**
   * Destroys command.
   */
  virtual ~ConnectCommand();

  /**
   * Executes command.
   *
   * Inhrited from Command abstract class.
   *
   * @throws IOException on io error, RemoteException on server side error.
   */
  virtual void execute() throw(IOException, RemoteException);
private:
  /**
   * Proxy to some of TightVNC server control methods.
   */
  ControlProxy *m_proxy;
  StringStorage m_hostName;
};

#endif // __CONNECTCOMMAND_H__
