// Copyright (C) 2008,2009,2010,2011,2012 GlavSoft LLC.
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

#ifndef __WINHDR_H__
#define __WINHDR_H__

#ifdef WINVER
#undef WINVER
#endif

#define WINVER 0x0501

#ifdef _WIN32_WINNT
#undef _WIN32_WINNT
#endif

#define _WIN32_WINNT 0x0501

#include <winsock2.h>
#include <ws2tcpip.h>
#include <wspiapi.h>
#include <windows.h>
#include <psapi.h>
#include <Wtsapi32.h>
#include <tchar.h>
#include <commctrl.h>
// hide annoying warning c:\program files(x86)\windows kits\8.1\include\um\dbghelp.h(1544) : warning C4091 : 'typedef ' : ignored on left of '' when no variable is declared
#pragma warning(push)
#pragma warning(disable:4091)
#include <Dbghelp.h>
#pragma warning(pop)
#include <ShellAPI.h>

#endif // __WINHDR_H__
