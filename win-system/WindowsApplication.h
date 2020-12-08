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

#ifndef __WINDOWSAPPLICATION_H__
#define __WINDOWSAPPLICATION_H__

#include "util/winhdr.h"
#include "thread/LocalMutex.h"

#include <list>

using namespace std;

/**
 * Base Windows Application class.
 * Have hidden main window and main message loop.
 */
class WindowsApplication
{
public:
  /**
   * Creates WindowsApplication instance.
   * @param appInstance parameter that passed to WinMain.
   */
  WindowsApplication(HINSTANCE appInstance, const TCHAR *windowClassName);

  /**
   * Destroys WindowsApplication instance.
   * @remark it does not shutdown application if it's executing it
   * separate thread.
   */
  virtual ~WindowsApplication();

  /**
   * Runs windows application.
   * @remark really it creates main window and starts windows message loop.
   * @return application exit code.
   */
  virtual int run();

  /**
   * Posts close and destroy message to main window.
   */
  virtual void shutdown();

  /**
   * Posts message to main window.
   */
  virtual void postMessage(UINT message, WPARAM wParam = 0, LPARAM lParam = 0);

  /**
   * Adds modeless dialog to application modeless dialog list to
   * enable switching between controls by pressing tab button.
   * @param dialogWindow HWND of modeless dialog.
   */
  static void addModelessDialog(HWND dialogWindow);

  /**
   * Removes dialog from application modeless dialog list.
   * @param dialogWindow HWND of modeless dialog.
   */
  static void removeModelessDialog(HWND dialogWindow);

protected:
  // Creates a window to receive messages.
  virtual void createWindow(const TCHAR *className);

  // Fills the wndClass argument and registers new class name in the Windows.
  virtual void registerWindowClass(WNDCLASS *wndClass);

  // Runs main messages process cycle. The run() function returns
  // value returned by this function.
  virtual int processMessages();

  /**
   * Windows prodecure for main application window.
   */
  static LRESULT CALLBACK wndProc(HWND hWnd, UINT msg, WPARAM wparam, LPARAM lparam);

  HINSTANCE m_appInstance;
  HWND m_mainWindow;
  StringStorage m_windowClassName;
protected:
  /**
   * Helper method to process modeless dialog message for modal dialog.
   * @param msg message to process.
   * @return true if don't need to translate and dispatch message in main message loop.
   */
  static bool processDialogMessage(MSG *msg);
private:
  static LocalMutex m_MDLMutex; // Modeless dialog list mutex.
  static list<HWND> m_modelessDialogList;
};

#endif // __WINDOWSAPPLICATION_H__
