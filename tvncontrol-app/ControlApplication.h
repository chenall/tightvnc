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

#ifndef _CONTROL_APPLICATION_H_
#define _CONTROL_APPLICATION_H_

#include "Transport.h"
#include "TransportFactory.h"

#include "util/CommonHeader.h"
#include "util/ResourceLoader.h"
#include "util/Command.h"
#include "server-config-lib/Configurator.h"
#include "log-writer/LogWriter.h"

#include "thread/Thread.h"
#include "thread/LocalMutex.h"

#include "tvncontrol-app/ControlGate.h"
#include "tvncontrol-app/ControlAuthException.h"

#include "ControlProxy.h"
#include "Notificator.h"

#include "win-system/WindowsApplication.h"

/**
 * External definition of tray icon.
 */
class ControlTrayIcon;

/**
 * Windows server control application.
 *
 * Contains code of wscontrol.exe.
 */
class ControlApplication : public WindowsApplication, /** Windows application running on current interactive desktop. */
                           public Notificator, /** Implement interface to show notifications. */
                           private Thread /** Status check thread. */
{
public:
  /**
   * Initializes all subsystems needed for wscontrol application and creates
   * ControlApplication instance.
   * @throws Exception if initialization of needed subsystems has failed.
   */
  ControlApplication(HINSTANCE hinst,
                     const TCHAR *windowClassName,
                     const TCHAR *commandLine);
  /**
   * Destructor of control application.
   */
  virtual ~ControlApplication();

  /**
   * Executes control application.
   * @returns exit code.
   */
  virtual int run();

  /**
   * Inherited from Notificator abstract class
   * @see Notification class for details.
   */
  virtual void notifyServerSideException(const TCHAR *reason);
  /**
   * Inherited from Notificator abstract class
   * @see Notification class for details.
   */
  virtual void notifyConnectionLost();

protected:
  /**
   * Connect to the control interface of the server. If both -controlservice
   * and -slave keys were specified, this function makes several tries to
   * connect, with a pause between tries. That should recover the case when
   * the control interface starts before the service is fully started.
   *
   * @param controlService true if -controlservice was specified in the
   *   command line, false otherwise. This flag is used to determine the name
   *   of the pipe to connect to, and affects error handling on connection
   *   failures (together with the slave flag).
   * @param slave true if -slave option was specified in the command line,
   *   false otherwise. This flag affects error handling on connection
   *   failures (together with the connectService flag).
   * @throws Exception on errors.
   */
  void connect(bool controlService, bool slave) throw(Exception);

  /**
   * Inherited from Thread class.
   *
   * Checks TightVNC server status.
   */
  void execute();

  /**
   * Runs control interface (tray icon).
   * @param showIcon determinates to show tray icon or not.
   * @return application exit code.
   */
  int runControlInterface(bool showIcon);
  /**
   * Runs control command (mode of tvncontrol).
   * @return application exit code.
   */
  int runControlCommand(Command *command);
  /**
   * Runs configuration dialog (mode of tvncontrol).
   * @param configService determinates if we gonna to configure service.
   * @param isRunAsRequested determinates if admin rights for tvncontrol was already requested
   * by parent process, ignored when configService is false.
   * @remark if configService is true and application don't have admin rights, then it
   * will start new process requesting admin rights.
   * @return application exit code.
   * @remark Call this function only to config in offline mode because it uses
   * the registry.
   */
  int runConfigurator(bool configService, bool isRunAsRequested);

  // Checks the rfb and administrator authentications. If one of them is empty
  // the function runs a dialog to prompt to enable the both authentication and
  // to set or to add passwords.
  int checkServicePasswords(bool isRunAsRequested);

private:
  /**
   * Converts plain text password to crypted password.
   * @fixme move in to tvnserver application as additional application.
   */
  static void getCryptedPassword(UINT8 cryptedPass[8], const TCHAR *plainTextPass);

  // Auxiliary function to the same name function. It does real work.
  void checkServicePasswords();

  // Auxiliary function that forces the current run service to reload configuration.
  void reloadConfig();

private:
  LogWriter m_log;

  // Initialization of the Configurator instance.
  Configurator m_configurator;

protected:

  /**
   * Low-level transport to control server.
   */
  Transport *m_transport;
  /**
   * Middle-level transport to control server.
   */
  ControlGate *m_gate;
  /**
   * High-level transport to control server (proxy).
   */
  ControlProxy *m_serverControl;
  /**
   * Control tray icon.
   */
  ControlTrayIcon *m_trayIcon;
  /**
   * Application command line.
   */
  StringStorage m_commandLine;
  /**
   * If application is in slave mode.
   */
  bool m_slaveModeEnabled;
  /**
   * Icon need to known if tvncontrol runs in slave mode
   * to modify menu.
   */
  friend class ControlTrayIcon;
};

#endif
