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

#ifndef _UPDATE_LOCAL_CONFIG_COMMAND_H_
#define _UPDATE_LOCAL_CONFIG_COMMAND_H_

#include "util/Command.h"

#include "ControlProxy.h"

/**
 * Command that copies current configuration of remote TightVNC server
 * to local application configuration.
 */
class UpdateLocalConfigCommand : public Command
{
public:
  /**
   * Creates command.
   * @param proxy ready to use control proxy.
   */
  UpdateLocalConfigCommand(ControlProxy *proxy);
  /**
   * Destructor.
   */
  virtual ~UpdateLocalConfigCommand();
  /**
   * Executes command.
   * @throws IOException, RemoteException.
   */
  virtual void execute() throw(IOException, RemoteException);
private:
  ControlProxy* m_proxy;
};

#endif
