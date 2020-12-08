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

#ifndef _CONTROL_AUTH_H_
#define _CONTROL_AUTH_H_

#include "tvncontrol-app/ControlGate.h"
#include "tvncontrol-app/ControlAuthException.h"

#include "util/Exception.h"

#include "server-config-lib/ServerConfig.h"

class ControlAuth
{
public:
  /**
   * Doesn't lock gate.
   */
  ControlAuth(ControlGate *gate, const TCHAR *password)
    throw(IOException, ControlAuthException, Exception);

  virtual ~ControlAuth();

protected:
  void authRfb() throw(IOException, ControlAuthException);

protected:
  ControlGate *m_gate;

  UINT8 m_password[ServerConfig::VNC_PASSWORD_SIZE];
};

#endif
