// Copyright (C) 2011,2012 GlavSoft LLC.
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

#ifndef _TVN_VIEWER_H_
#define _TVN_VIEWER_H_

#include "AboutDialog.h"
#include "ConfigurationDialog.h"
#include "ConnectionListener.h"
#include "ControlTrayIcon.h"
#include "LoginDialog.h"
#include "OptionsDialog.h"
#include "ViewerCollector.h"
#include "win-system/WindowsApplication.h"

#include "log-writer/LogWriter.h"
#include "thread/AutoLock.h"

#include <map>

class ViewerCollector;
class ControlTrayIcon;
class LoginDialog;

class TvnViewer : public WindowsApplication
{
public:
  TvnViewer(HINSTANCE appInstance,
            const TCHAR *windowClassName,
            const TCHAR *viewerWindowClassName);
  virtual ~TvnViewer();

  //
  // show login dialog
  //
  void showLoginDialog();

  //
  // show options for listening mode
  //
  void showListeningOptions();

  //
  // show dialog "About of Viewer"
  //

  void showAboutViewer();

  //
  // show dialog with configuration of viewer
  //
  void showConfiguration();

  // newConnection(...) and startListening(...) always do copy of params (StringStorage,
  // ConnectionData and ConnectionConfig). After call this function can free memory
  // with hostName, connectionData, connectionConfig
  void newListeningConnection();
  void newConnection(const StringStorage *hostName, const ConnectionConfig *connectionConfig);
  void newConnection(const ConnectionData *conData, const ConnectionConfig *connectionConfig);
  void startListening(int listeningPort);
  void stopListening();

  // Inherited from WindowsApplication
  void registerWindowClass(WNDCLASS *wndClass);
  static LRESULT CALLBACK wndProc(HWND hWnd, UINT msg, WPARAM wparam, LPARAM lparam);
  virtual void createWindow(const TCHAR *className);
  int processMessages();

public:
  // this message must sended after accepted new listening-connection
  static const int WM_USER_NEW_LISTENING = WM_USER + 1;

  // this message need send if you need show login dialog
  static const int WM_USER_SHOW_LOGIN_DIALOG = WM_USER + 2;

  // this message need send if you need show configuration dialog
  static const int WM_USER_CONFIGURATION = WM_USER + 3;

  // this message need send if you need show about dialog
  static const int WM_USER_ABOUT = WM_USER + 4;

  // This message need send if you need reconnect to host.
  // LPARAM contained pointer to ConnectionData.
  static const int WM_USER_RECONNECT = WM_USER + 5;

  // This message need send if config is changed.
  static const int WM_USER_CONFIGURATION_RELOAD = WM_USER + 6;

  // This timer is used for deleting dead instances of viewer.
  static const int TIMER_DELETE_DEAD_INSTANCE = 1;
  static const int TIMER_DELETE_DEAD_INSTANCE_DELAY = 50;

protected:
  void startListeningServer(const int listeningPort);
  void stopListeningServer();
  void restartListeningServer();

  void registerViewerWindowClass();
  void unregisterViewerWindowClass();
  static LRESULT CALLBACK wndProcViewer(HWND hWnd, UINT msg, WPARAM wparam, LPARAM lparam);

  void runInstance(ConnectionData *conData, const ConnectionConfig *config);
  void runInstance(const StringStorage *hostName, const ConnectionConfig *config);

  // This method return true, if login dialog is visible.
  bool isVisibleLoginDialog() const;

  bool onTimer(WPARAM idTimer);

  ViewerCollector m_instances;

  // class name of viewer-window
  StringStorage m_viewerWindowClassName;

  HACCEL m_hAccelTable;

private:
  void addInstance(ViewerInstance *viewerInstance);

  bool m_isListening;
  
  LogWriter m_logWriter;

  AboutDialog m_aboutDialog;
  ConfigurationDialog m_configurationDialog;
  OptionsDialog m_optionsDialog;

  LoginDialog *m_loginDialog;
  ControlTrayIcon *m_trayIcon;
  ConnectionListener *m_conListener;

  WNDCLASS m_viewerWndClass;
};

#endif
