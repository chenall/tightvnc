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

#include "ControlTrayIcon.h"
#include "ResourceStrings.h"
#include "gui/BaseWindow.h"

UINT ControlTrayIcon::WM_USER_TASKBAR;

ControlTrayIcon::ControlTrayIcon(TvnViewer *viewerApplication)
: NotifyIcon(false),
  m_application(viewerApplication),
  m_inWindowProc(false),
  m_icon(IDI_APPICON)
{
  ResourceStrings resStr;
  m_menu.createPopupMenu();
  m_menu.appendMenu(resStr.getStrRes(IDS_TB_NEWCONNECTION), IDS_NEW_CONN);
  m_menu.appendSeparator();
  m_menu.appendMenu(resStr.getStrRes(IDS_LISTENING_OPTIONS), IDS_LISTENING_OPTIONS);
  m_menu.appendMenu(resStr.getStrRes(IDS_CONFIG), IDS_CONFIG);
  m_menu.appendSeparator();
  m_menu.appendMenu(resStr.getStrRes(IDS_ABOUT_VIEWER), IDS_ABOUT_VIEWER);
  m_menu.appendSeparator();
  m_menu.appendMenu(resStr.getStrRes(IDS_CLOSE), IDS_CLOSE);
  m_menu.setDefaultItem(IDS_NEW_CONN);

  setWindowProcHolder(this);

  WM_USER_TASKBAR = RegisterWindowMessage(_T("TaskbarCreated"));
}

ControlTrayIcon::~ControlTrayIcon()
{
}

LRESULT ControlTrayIcon::windowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool *useDefWindowProc)
{
  if (m_inWindowProc) {
    // This call is recursive, do not do any real work.
    *useDefWindowProc = true;
    return 0;
  }
  // Make sure to reset it back to false before leaving this function for any
  // reason (check all return statements, exceptions should not happen here).
  m_inWindowProc = true;

  switch (uMsg) {
  case WM_USER + 1:
    switch (lParam) {
    case WM_RBUTTONUP:
      onRightButtonUp();
      break;
    case WM_LBUTTONDOWN:
      onLeftButtonDown();
      break;
    } // switch (lParam)
    break;
  default:
    if (uMsg == WM_USER_TASKBAR) {
      if (isVisible()) {
        hide();
        show();
      }
      break;
    }
    *useDefWindowProc = true;
  }

  m_inWindowProc = false;
  return 0;
}

void ControlTrayIcon::showIcon()
{
  show();
  setIcon(&m_icon);
}

void ControlTrayIcon::onRightButtonUp()
{
  POINT pos;

  if (!GetCursorPos(&pos)) {
    pos.x = pos.y = 0;
  }

  HWND notifyWnd = getWindow();
  SetForegroundWindow(notifyWnd);

  int action = TrackPopupMenu(m_menu.getMenu(),
                              TPM_NONOTIFY | TPM_RETURNCMD | TPM_RIGHTBUTTON,
                              pos.x, pos.y, 0, notifyWnd, NULL);

  switch (action) {
  case IDS_NEW_CONN:
    onNewConnection();
    break;
  case IDS_LISTENING_OPTIONS:
    onListeningOptions();
    break;
  case IDS_CONFIG:
    onConfiguration();
    break;
  case IDS_ABOUT_VIEWER:
    onAboutViewer();
    break;
  case IDS_CLOSE:
    onCloseListeningDaemon();
    break;
  default:
    _ASSERT(true);
  }
}

void ControlTrayIcon::onLeftButtonDown()
{
  onShowMainWindow();
}

void ControlTrayIcon::onShowMainWindow()
{
  onNewConnection();
}

void ControlTrayIcon::onNewConnection()
{
  m_application->showLoginDialog();
}

void ControlTrayIcon::onListeningOptions()
{
  m_application->showListeningOptions();
}

void ControlTrayIcon::onConfiguration()
{
  m_application->showConfiguration();
}

void ControlTrayIcon::onAboutViewer()
{
  m_application->showAboutViewer();
}

void ControlTrayIcon::onCloseListeningDaemon()
{
  m_application->stopListening();
}
