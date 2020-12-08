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

#ifndef _TVN_SERVICE_H_
#define _TVN_SERVICE_H_

#include "TvnServer.h"
#include "TvnServerListener.h"
#include "log-server/LogServer.h"
#include "log-server/ClientLogger.h"
#include "win-system/Service.h"

#include "thread/Thread.h"
#include "WinServiceEvents.h"
#include "NewConnectionEvents.h"

/**
 * TightVNC service.
 *
 * Solves problems of installing, reinstalling, removing, starting, stopping tvnserver
 * from service control manager.
 *
 * Also contains body of tvnservice.
 */
class TvnService : public Service,
                   public TvnServerListener,
                   private LogInitListener
{
public:
  /**
   * Command line key which needed to start tvnserver binary as service.
   */
  static const TCHAR SERVICE_COMMAND_LINE_KEY[];
public:
  /**
   * Creates object.
   */
  TvnService(WinServiceEvents *winServiceEvents,
             NewConnectionEvents *newConnectionEvents);
  /**
   * Deletes object.
   */
  virtual ~TvnService();

  /**
   * Inherited from abstract TvnServerListener class.
   * Shutdows tvnservice using service control manager.
   */
  virtual void onTvnServerShutdown();

  /**
   * Installs tvnserver service.
   * @throws SystemException on fail.
   */
  static void install() throw(SystemException);
  /**
   * Stops and removes tvnserver service.
   * @throws SystemException when failed to remove service.
   */
  static void remove() throw(SystemException);
  /**
   * Reinstalls tvnserver service (combite call of remove and install methods).
   * @remark ignores if remove call throws exception.
   * @throws SystemException when fail to install service.
   */
  static void reinstall() throw(SystemException);
  /**
   * Starts tvnserver service.
   * @param waitCompletion if true, wait until the status becomes
   *   SERVICE_RUNNING.
   * @throws SystemException on fail.
   */
  static void start(bool waitCompletion = false) throw(SystemException);
  /**
   * Stopps tvnserver service.
   * @param waitCompletion if true, wait until the status becomes
   *   SERVICE_STOPPED.
   * @throws SystemException on fail.
   */
  static void stop(bool waitCompletion = false) throw(SystemException);

protected:
  /**
   * Inherited from superclass.
   * Starts tvnserver execution.
   * @throws SystemException when failed to start.
   */
  virtual void onStart() throw(SystemException);

  /**
   * Inherited from superclass.
   * TvnService working body.
   */
  virtual void main();

  /**
   * Inherited from superclass.
   * Stops tvnserver execution.
   */
  virtual void onStop();

  /**
   * Creates path to binary of tvnserver service.
   * @param binPath [out] output string where path will be stored.
   * @return true on success, false on error.
   * @remark creates path with all needed keys for starting service.
   */
  static bool getBinPath(StringStorage *binPath);

  // This is a callback function that calls when the log can be initialized.
  virtual void onLogInit(const TCHAR *logDir, const TCHAR *fileName, unsigned char logLevel);

  // This is a callback function that calls when log properties have changed.
  virtual void onChangeLogProps(const TCHAR *newLogDir, unsigned char newLevel);

protected:
  /**
   * Shutdown service event.
   */
  WindowsEvent m_shutdownEvent;
  /**
   * TightVNC server.
   */
  TvnServer *m_tvnServer;

  LogServer m_logServer;
  ClientLogger m_clientLogger;

  WinServiceEvents *m_winServiceEvents;
  NewConnectionEvents *m_newConnectionEvents;
};

#endif
