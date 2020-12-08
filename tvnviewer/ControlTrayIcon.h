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

#ifndef _CONTROL_TRAY_ICON_H_
#define _CONTROL_TRAY_ICON_H_

#include "gui/NotifyIcon.h"
#include "gui/Menu.h"
#include "TvnViewer.h"

#include "resource.h"

class TvnViewer;

class ControlTrayIcon : public NotifyIcon,
                        public WindowProcHolder
{
public:
  ControlTrayIcon(TvnViewer *viewerApplication);
  virtual ~ControlTrayIcon();

  //
  // this function set icon and show icon [call setIcon(), show()]
  //
  void showIcon();

protected:
  static UINT WM_USER_TASKBAR;

protected:
  virtual LRESULT windowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool *useDefWindowProc);

  //
  // Show login-dialog after click "New connection..." in pop-up menu of tray icon
  //
  virtual void onNewConnection();
  
  //
  // Show dialog with connection options after click
  // "Options for incoming connection..." in pop-up menu of tray icon
  //
  virtual void onListeningOptions();

  //
  // Show dialog with configuration of viewer
  //
  virtual void onConfiguration();
  
  //
  // Show about-dialog of viewer
  //
  virtual void onAboutViewer();

  //
  // Stopping of listening daemon after click
  // "Close listening daemon" in pop-up menu of tray icon
  //
  virtual void onCloseListeningDaemon();

  //
  // Show login-dialog after click on tray icon
  //
  virtual void onShowMainWindow();

  Icon m_icon;
  Menu m_menu;
  bool m_inWindowProc;

  TvnViewer *m_application;

private:
  void onRightButtonUp();
  void onLeftButtonDown();
};

#endif
