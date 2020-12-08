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

#include "rfb/AuthDefs.h"

const char *const AuthDefs::SIG_NONE = "NOAUTH__";
const char *const AuthDefs::SIG_VNC = "VNCAUTH_";
const char *const AuthDefs::SIG_EXTERNAL = "XTRNAUTH";

UINT32 SecurityDefs::convertFromAuthType(UINT32 authType)
{
  switch (authType) {
  case AuthDefs::NONE:
    return NONE;
  case AuthDefs::VNC:
    return VNC;
  }

  return INVALID; // no corresponding authentication method
}

UINT32 AuthDefs::convertFromSecurityType(UINT32 securityType)
{
  switch (securityType) {
  case SecurityDefs::NONE:
    return NONE;
  case SecurityDefs::VNC:
    return VNC;
  }

  return 0; // no corresponding authentication method
}
