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

#ifndef __HOOKINSTALLER_H__
#define __HOOKINSTALLER_H__

#include "util/CommonHeader.h"
#include "win-system/DynamicLibrary.h"

class HookInstaller
{
public:
  HookInstaller();
  ~HookInstaller();

  // Instals hooks to windowed applications.
  // targedWinHwnd - is hwnd to a target window that will receive
  // hooked messages.
  void install(HWND targedWinHwnd);
  // Uninstals hooks from windowed applications.
  void uninstall();

private:
  static const TCHAR LIBRARY_NAME[];
  static const char SET_HOOK_FUNCTION_NAME[];
  static const char UNSET_HOOK_FUNCTION_NAME[];

  DynamicLibrary m_lib;
  FARPROC m_pSetHook;
  FARPROC m_pUnSetHook;
};

#endif // __HOOKINSTALLER_H__
