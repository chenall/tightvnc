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

#ifndef _MAKE_RFB_CONNECTION_COMMAND_H_
#define _MAKE_RFB_CONNECTION_COMMAND_H_

#include "util/Command.h"

#include "ControlProxy.h"

/**
 * Command that initializes outgoing rfb connection from TightVNC server to
 * specified address.
 */
class MakeRfbConnectionCommand : public Command
{
public:
  /**
   * Creates command.
   * @param serverControl proxy.
   * @param connectString connection string.
   * @param viewOnly view only flag for outgoing connection.
   */
  MakeRfbConnectionCommand(ControlProxy *serverControl,
                           const TCHAR *connectString,
                           bool viewOnly);
  /**
   * Destroys command.
   */
  virtual ~MakeRfbConnectionCommand();

  /**
   * Executes command.
   *
   * Inhrited from Command abstract class.
   *
   * @throws IOException on io error, Exception on server side error.
   */
  virtual void execute() throw(IOException, RemoteException);

private:
  /**
   * Proxy to some of TightVNC server control methods.
   */
  ControlProxy *m_proxy;
  /**
   * Connection string.
   */
  StringStorage m_connectString;
  /**
   * View only flag for outgoing connection.
   */
  bool m_viewOnly;
};

#endif
