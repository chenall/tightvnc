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

#ifndef _WINSOCK_CLASS_H_
#define _WINSOCK_CLASS_H_

#include "util/CommonHeader.h"
#include "util/Exception.h"

//
// Class that startup and cleanup Windows Sockets subsystem.
//
class WindowsSocket
{
public:
  // Initializes Windows sockets subsystem.
  // Throws exception if winsock already initialized or
  // if was error during winsock startup.
  static void startup(BYTE loVer, BYTE hiVer) throw(Exception);

  //  Deinitializes Windows sockets subsystem.
  // Throws exception if winsock does not initialized or
  // if was error during winsock cleanup.
  static void cleanup() throw(Exception);

protected:
  static bool m_isStarted;
};

#endif
