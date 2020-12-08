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
#include "gui/WindowProcHolder.h"
#include "gui/drawing/Icon.h"

#include "thread/Thread.h"
#include "thread/LocalMutex.h"

#include "util/Command.h"
#include "util/MacroCommand.h"

#include "wsconfig-lib/ConfigDialog.h"

#include "ControlProxy.h"
#include "Notificator.h"
#include "ControlApplication.h"
#include "AboutDialog.h"

/**
 * TvnControl application icon class.
 */
class ControlTrayIcon : public NotifyIcon, /** Inherit tray icon code. */
                        public WindowProcHolder /** To override tray icon window procedure. */
{
public:
  /**
   * Creates control tray icon and places it to system tray.
   * @param serverControl proxy to execute methods in TightVNC server process.
   * @param notificator interface to report about errors during execution of remote methods.
   * @param appControl parent control application.
   * @param showAfterCreation determinates if needs to show icon in tray.
   */
  ControlTrayIcon(ControlProxy *serverControl,
                  Notificator *notificator,
                  ControlApplication *appControl,
                  bool showAfterCreation);
  /**
   * Destroys tray icon.
   */
  virtual ~ControlTrayIcon();

  /**
   * Synchronizes tray icon and status text with TightVNC server.
   * @remark method shutdowns control application if connection to
   * TightVNC server is lost.
   */
  void syncStatusWithServer();

  // Terminates all function callings and then notifying to the
  // function waitForTermination() to continue.
  void terminate();

  // Wait termination of using a function by windows (e.g. windowProc) and then
  // continue. Don't use this function from thread which call the windowProc()
  // function.
  void waitForTermination();

protected:
  /**
   * Sets icon state to "not connected to server".
   */
  void setNotConnectedState();

  /**
   * Inherited from WindowProcHolder class.
   *
   * Overrides default tray icon window behavour.
   */
  virtual LRESULT windowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool *useDefWindowProc);

  /**
   * Handlers of tray icon window events.
   */
  void onRightButtonUp();
  void onLeftButtonDown();

  /**
   * Tray icon popup menu items message handlers.
   */
  void onConfigurationMenuItemClick();
  void onDisconnectAllClientsMenuItemClick();
  void onShutdownServerMenuItemClick();
  void onOutgoingConnectionMenuItemClick();
  void onAttachToDispatcher();
  void onAboutMenuItemClick();
  void onCloseControlInterfaceMenuItemClick();

protected:
  static UINT WM_USER_TASKBAR;

protected:

  // Interface to show error notifications.
  Notificator *m_notificator;

  // Pointer to control application.
  ControlApplication *m_appControl;

  // States of tray icon.
  Icon *m_iconWorking;
  Icon *m_iconIdle;
  Icon *m_iconDisabled;

  // Interface to execute some commands on remote TightVNC server.
  ControlProxy *m_serverControl;

  // Configuration dialog.
  ConfigDialog *m_configDialog;
  // About dialog.
  AboutDialog m_aboutDialog;

  // Last known TightVNC server information.
  TvnServerInfo m_lastKnownServerInfo;
  // Thread-safety of m_lastKnownServerInfo member.
  LocalMutex m_serverInfoMutex;

  // Commands for configuration dialog.
  Command *m_updateRemoteConfigCommand;
  Command *m_updateLocalConfigCommand;
  MacroCommand *m_applyChangesMacroCommand;
  ControlCommand *m_applyChangesControlCommand;

  // This variable is set to true when entering ControlTrayIcon::windowProc(),
  // and is used to prevent from executing that function recursively.
  bool m_inWindowProc;

  WindowsEvent m_endEvent;
  bool m_termination;
};

#endif
