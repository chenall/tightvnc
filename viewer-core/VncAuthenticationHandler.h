// Copyright (C) 2012 GlavSoft LLC.
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

#ifndef _VNC_AUTHENTICATION_HANDLER_H_
#define _VNC_AUTHENTICATION_HANDLER_H_

#include "viewer-core/AuthHandler.h"

class VncAuthenticationHandler : public AuthHandler
{
public:
  VncAuthenticationHandler();
  virtual ~VncAuthenticationHandler();

  //
  // Overrides AuthHandler::authenticate().
  //
  virtual void authenticate(DataInputStream *input, DataOutputStream *output);

  //
  // Overrides AuthHandler::addAuthCapability().
  //
  virtual void addAuthCapability(CapabilitiesManager *capabilitiesManager);

protected:
  virtual void getPassword(StringStorage *passString) = 0;
};

#endif
