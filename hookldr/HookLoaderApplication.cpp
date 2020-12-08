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

#include "HookLoaderApplication.h"
#include "util/StringParser.h"
#include "tvnserver-app/NamingDefs.h"
#include "desktop/HookInstaller.h"

HookLoaderApplication::HookLoaderApplication(HINSTANCE appInstance,
                                             const TCHAR *windowClassName,
                                             const TCHAR *commandLine)
: WindowsApplication(appInstance, windowClassName),
  m_commandLine(commandLine)
{
  m_parentWatcher = new ParentWatcher(this);
}

HookLoaderApplication::~HookLoaderApplication()
{
  delete m_parentWatcher;
}

void HookLoaderApplication::registerWindowClass(WNDCLASS *wndClass)
{
  memset(wndClass, 0, sizeof(WNDCLASS));

  // Set default values. Derived classes can redefine this fields
  wndClass->lpfnWndProc = hookLoaderWndProc;
  wndClass->hInstance = m_appInstance;
  wndClass->lpszClassName = HookDefinitions::HOOK_LOADER_WINDOW_CLASS;

  RegisterClass(wndClass);
}

int HookLoaderApplication::processMessages()
{
  // m_commandLine - is a target window hwnd by 64-bit text number.
  UINT64 targetWinValue = 0;
  HWND targetWinHwnd = 0;
  if (!StringParser::parseUInt64(m_commandLine.getString(),
                                 &targetWinValue)) {
    throw Exception(_T("HookLoaderApplication failed: Can't")
                    _T(" parse the command line to UINT64"));
  }
  targetWinHwnd = (HWND)targetWinValue;

  HookInstaller hookInstaller;
  hookInstaller.install(targetWinHwnd);

  MSG msg;
  BOOL ret;
  while (ret = GetMessage(&msg, NULL, 0, 0) != 0) {
    if (ret < 0) {
      return 1;
    }
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }
  return 0;
}

LRESULT CALLBACK HookLoaderApplication::hookLoaderWndProc(HWND hWnd,
                                                          UINT msg,
                                                          WPARAM wparam,
                                                          LPARAM lparam)
{
  if (msg == HookDefinitions::LOADER_CLOSE_CODE) {
    PostMessage(hWnd, WM_CLOSE, 0, 0);
    return 0;
  }

  switch (msg) {
  case WM_CLOSE:
    DestroyWindow(hWnd);
    return 0;
  case WM_DESTROY:
    PostQuitMessage(0);
    return 0;
  }
  return DefWindowProc(hWnd, msg, wparam, lparam);
}

void HookLoaderApplication::onParentTerminate()
{
  shutdown();
}
