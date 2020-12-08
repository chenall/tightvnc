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

#ifndef _TVN_SERVER_H_
#define _TVN_SERVER_H_

#include "util/CommonHeader.h"

#include "desktop/WinServiceDesktopFactory.h"
#include "desktop/ApplicationDesktopFactory.h"
#include "RfbClientManager.h"
#include "RfbServer.h"
#include "ExtraRfbServers.h"
#include "ControlServer.h"
#include "TvnServerListener.h"

#include "http-server-lib/HttpServer.h"

#include "thread/ZombieKiller.h"
#include "thread/LocalMutex.h"
#include "log-writer/LogWriter.h"
#include "util/Singleton.h"
#include "util/ListenerContainer.h"
#include "NewConnectionEvents.h"

#include "server-config-lib/Configurator.h"

#include "tvncontrol-app/TvnServerInfo.h"
#include "LogInitListener.h"

/**
 * TightVNC server singleton that includes serveral components:
 *   1) Zombie killer singleton.
 *   2) Configurator singleton.
 *   3) Log singleton.
 *   4) Rfb servers (main rfb server and extra servers).
 *   5) Http server.
 *   6) Control server.
 *   7) Other features:
     1) Do action when last client disconnects.
 */
class TvnServer : public Singleton<TvnServer>,
                  public ListenerContainer<TvnServerListener *>,
                  public ConfigReloadListener,
                  public RfbClientManagerEventListener
{
public:
  /**
   * Creates and starts TightVNC server execution (in separate thread).
   *
   * Makes sereval steps:
   *  1) Instanizes zombie killer.
   *  2) Instanizes configurator and load configuration.
   *  3) Instanizes log.
   *  4) Starts all servers.
   *
   * @param runsInServiceContext must be set to true if TvnServer is running in service context,
   * false, if in context of single application. Parameter determinates control client behavour and
   * initial place for loading TightVNC configuration.
   *
   * @remark doesn't block calling thread execution cause all servers runs in it's own threads.
   * To know when need to shutdown TightVNC server you need to use addListener method.
   */
  TvnServer(bool runsInServiceContext,
            NewConnectionEvents *newConnectionEvents,
            LogInitListener *logInitListener,
            Logger *logger);
  /**
   * Stops and destroys TightVNC server.
   * @remark don't generate shutdown signal(like shutdown() method does) for listeners.
   */
  virtual ~TvnServer();

  /**
   * Fills structure with information of current state of TvnServer.
   * @param info [out] output parameter that will contain TightVNC server information
   * after call of this method.
   * @fixme place extended information to server info.
   */
  void getServerInfo(TvnServerInfo *info);

  /**
   * Inherited from ConfigReloadListener interface to catch configuration reload event.
   *
   * Make several things:
   *  1) Changes log level.
   *  2) Restarts rfb servers.
   *  3) Restarts http server.
   */
  virtual void onConfigReload(ServerConfig *serverConfig);

  /**
   * Only generates shutdown signal (event) for TvnServer listeners.
   *
   * @remark used by ControlClient, when it recieves command to shutdown TightVNC.
   * @remark doesn't stop TightVNC server.
   * @fixme rename this method.
   */
  void generateExternalShutdownSignal();

  /**
   * Checks if TightVNC server runs in service context.
   * @returns true if runs in service context.
   * @deprecated use getServerInfo() instead or move to private.
   */
  bool isRunningAsService() const;

  /**
   * Implemented from RfbClientManagerEventListener.
   *
   * Does nothing.
   */
  virtual void afterFirstClientConnect();

  /**
   * Implemented from RfbClientManagerEventListener.
   *
   * Does specifed in configuration action when last client disconnects from
   * rfb server.
   */
  virtual void afterLastClientDisconnect();

protected:
  void restartHttpServer();
  void restartControlServer();
  void restartMainRfbServer();

  void stopHttpServer();
  void stopControlServer();
  void stopMainRfbServer();

  // Calls a callback function to change update log properties.
  void changeLogProps();

protected:
  LogWriter m_log;
  ZombieKiller m_zombieKiller;

  Configurator m_config;
  /**
   * Shortcut to global server configuration.
   */
  ServerConfig *m_srvConfig;

  /**
   * Mutex for protecting servers.
   */
  LocalMutex m_mutex;

  /**
   * Flag that determitates if we run in server context.
   * true if service, false if application.
   */
  const bool m_runAsService;

  WinServiceDesktopFactory m_serviceDesktopFactory;
  ApplicationDesktopFactory m_applicationDesktopFactory;
  /**
   * Rfb client manager (for all rfb servers), used by rfb servers
   * rfb clients, control server and control clients.
   */
  RfbClientManager *m_rfbClientManager;
  /**
   * Control server.
   */
  ControlServer *m_controlServer;
  /**
   * Builtin http server.
   */
  HttpServer *m_httpServer;
  /**
   * Main rfb server.
   */
  RfbServer *m_rfbServer;
  /**
   * Extra servers for extra ports. This object is not protected by any mutex
   * and it does not implement any internal locking, so it should be used with
   * caution. Here we change its state on owner creation, on owner deletion
   * and on each configuration change (via a listener function called from
   * other threads). The listener function is registered after the object
   * creation and unregistered before the owner destruction, and its calls are
   * properly synchronized. Thus, we can be sure that m_extraRfbServers is not
   * used by different threads simultaneously.
   */
  ExtraRfbServers m_extraRfbServers;

  LogInitListener *m_logInitListener;

  UINT m_contextSwitchResolution; // in ms
};

#endif
