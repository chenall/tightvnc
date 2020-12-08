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

#include "HookInstaller.h"

#ifndef _WIN64
const TCHAR HookInstaller::LIBRARY_NAME[] = _T("screenhooks32.dll");
#else
const TCHAR HookInstaller::LIBRARY_NAME[] = _T("screenhooks64.dll");
#endif

const char HookInstaller::SET_HOOK_FUNCTION_NAME[] = "setHook";
const char HookInstaller::UNSET_HOOK_FUNCTION_NAME[] = "unsetHook";

typedef bool (*SetHookFunction)(HWND targedWinHwnd);
typedef bool (*UnsetHookFunction)();

HookInstaller::HookInstaller()
: m_lib(LIBRARY_NAME),
  m_pSetHook(0),
  m_pUnSetHook(0)
{
}

HookInstaller::~HookInstaller()
{
  uninstall();
}

void HookInstaller::install(HWND targedWinHwnd)
{
  HINSTANCE hinst = GetModuleHandle(0);

  m_pSetHook = m_lib.getProcAddress(SET_HOOK_FUNCTION_NAME);
  m_pUnSetHook = m_lib.getProcAddress(UNSET_HOOK_FUNCTION_NAME);
  if (!m_pSetHook || !m_pUnSetHook) {
    throw Exception(_T("Cannot find the setHook() and unsetHook() functions"));
  }

  // Hooks initializing
  SetHookFunction setHookFunction = (SetHookFunction)m_pSetHook;
  if (!setHookFunction(targedWinHwnd)) {
    throw Exception(_T("setHook() function failed"));
  }
}

void HookInstaller::uninstall()
{
  if (m_pUnSetHook) {
    UnsetHookFunction unsetHookFunction = (UnsetHookFunction)m_pUnSetHook;
    if (!unsetHookFunction()) {
      throw Exception(_T("unsetHook() function failed"));
    }
    m_pUnSetHook = 0;
    m_pSetHook = 0;
  }
}
