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

#ifndef __HOOKLOADERAPPLICATION_H__
#define __HOOKLOADERAPPLICATION_H__

#include "win-system/LocalWindowsApplication.h"
#include "ParentWatcher.h"

class HookLoaderApplication : public WindowsApplication,
                              private ParentEventsListener
{
public:
  HookLoaderApplication(HINSTANCE appInstance,
                        const TCHAR *windowClassName,
                        const TCHAR *commandLine);
  virtual ~HookLoaderApplication();

private:
  virtual void onParentTerminate();

  virtual void registerWindowClass(WNDCLASS *wndClass);
  virtual int processMessages();

  static LRESULT CALLBACK hookLoaderWndProc(HWND hWnd, UINT msg,
                                            WPARAM wparam,
                                            LPARAM lparam);

  StringStorage m_commandLine;

  ParentWatcher *m_parentWatcher;
};

#endif // __HOOKLOADERAPPLICATION_H__
