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

#ifndef __RFB_AUTH_DEFS_H_INCLUDED__
#define __RFB_AUTH_DEFS_H_INCLUDED__

#include "util/inttypes.h"

class SecurityDefs
{
public:
  static const UINT32 INVALID = 0;
  static const UINT32 NONE = 1;
  static const UINT32 VNC = 2;
  static const UINT32 TIGHT = 16;
  static UINT32 convertFromAuthType(UINT32 authType);
};

class AuthDefs
{
public:
  static const UINT32 NONE = 1;
  static const UINT32 VNC = 2;
  static const UINT32 EXTERNAL = 130;

  static const char *const SIG_NONE;
  static const char *const SIG_VNC;
  static const char *const SIG_EXTERNAL;

  // Return TightVNC authentication method corresponding to a VNC-style
  // security type. Returns 0 if the specified security type does not map
  // to any valid authentication type supported in TightVNC.
  static UINT32 convertFromSecurityType(UINT32 securityType);
};

#endif // __RFB_AUTH_DEFS_H_INCLUDED__
