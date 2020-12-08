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

#ifndef _UPDATE_REMOTE_CONFIG_COMMAND_H_
#define _UPDATE_REMOTE_CONFIG_COMMAND_H_

#include "util/Command.h"

#include "ControlProxy.h"

/**
 * Command that sends current configuration to TightVNC Server.
 * This command will ask server to save recieved configuration.
 * @fixme: use MacroCommand to join UpdateRemoteConfigCommand and UpdateLocalConfigCommand.
 */
class UpdateRemoteConfigCommand : public Command
{
public:
  /**
   * Creates command.
   * @param serverControl control proxy.
   */
  UpdateRemoteConfigCommand(ControlProxy *serverControl);
  /**
   * Deletes command.
   */
  virtual ~UpdateRemoteConfigCommand();

  /**
   * Inherited from Command interface.
   *
   * @throws IOException on io error, RemoteException on server side error.
   */
  virtual void execute() throw(IOException, RemoteException);

protected:
  /**
   * Control proxy on client side.
   */
  ControlProxy *m_serverControl;
};

#endif
