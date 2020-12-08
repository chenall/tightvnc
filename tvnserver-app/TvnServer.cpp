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

#include "TvnServer.h"
#include "WsConfigRunner.h"
#include "AdditionalActionApplication.h"
#include "win-system/CurrentConsoleProcess.h"
#include "win-system/Environment.h"

#include "server-config-lib/Configurator.h"

#include "thread/GlobalMutex.h"

#include "tvnserver/resource.h"

#include "wsconfig-lib/TvnLogFilename.h"

#include "network/socket/WindowsSocket.h"

#include "util/StringTable.h"
#include "util/AnsiStringStorage.h"
#include "tvnserver-app/NamingDefs.h"

#include "file-lib/File.h"

// FIXME: Bad dependency on tvncontrol-app.
#include "tvncontrol-app/TransportFactory.h"
#include "tvncontrol-app/ControlPipeName.h"

#include "tvnserver/BuildTime.h"

#include <crtdbg.h>
#include <time.h>
#include "TimeAPI.h"


TvnServer::TvnServer(bool runsInServiceContext,
                     NewConnectionEvents *newConnectionEvents,
                     LogInitListener *logInitListener,
                     Logger *logger)
: Singleton<TvnServer>(),
  ListenerContainer<TvnServerListener *>(),
  m_runAsService(runsInServiceContext),
  m_logInitListener(logInitListener),
  m_rfbClientManager(0),
  m_httpServer(0), m_controlServer(0), m_rfbServer(0),
  m_config(runsInServiceContext),
  m_log(logger),
  m_contextSwitchResolution(1),
  m_extraRfbServers(&m_log)
{
  m_log.message(_T("%s Build on %s"),
                 ProductNames::SERVER_PRODUCT_NAME,
                 BuildTime::DATE);

  // Initialize configuration.
  // FIXME: It looks like configurator may be created as a member object.
  Configurator *configurator = Configurator::getInstance();
  configurator->load();
  m_srvConfig = Configurator::getInstance()->getServerConfig();

  try {
    StringStorage logDir;
    m_srvConfig->getLogFileDir(&logDir);
    unsigned char logLevel = m_srvConfig->getLogLevel();
    // FIXME: Use correct log name.
    m_logInitListener->onLogInit(logDir.getString(), LogNames::SERVER_LOG_FILE_STUB_NAME, logLevel);

  } catch (...) {
    // A log error must not be a reason that stop the server.
  }

  // Initialize windows sockets.

  m_log.info(_T("Initialize WinSock"));

  try {
    WindowsSocket::startup(2, 1);
  } catch (Exception &ex) {
    m_log.interror(_T("%s"), ex.getMessage());
  }

  DesktopFactory *desktopFactory = 0;
  if (runsInServiceContext) {
    desktopFactory = &m_serviceDesktopFactory;
  } else {
    desktopFactory = &m_applicationDesktopFactory;
  }

  m_rfbClientManager = new RfbClientManager(0, newConnectionEvents, &m_log, desktopFactory);

  m_rfbClientManager->addListener(this);

  // FIXME: No good to act as a listener before completing the object
  //        construction.
  Configurator::getInstance()->addListener(this);

  {
    // FIXME: Protect only primitive operations.
    // FIXME: Nested lock in protected code (congifuration locking).
    AutoLock l(&m_mutex);

    restartMainRfbServer();
    (void)m_extraRfbServers.reload(m_runAsService, m_rfbClientManager);
    restartHttpServer();
    restartControlServer();
  }
}

TvnServer::~TvnServer()
{
  Configurator::getInstance()->removeListener(this);

  stopControlServer();
  stopHttpServer();
  m_extraRfbServers.shutDown();
  stopMainRfbServer();

  ZombieKiller *zombieKiller = ZombieKiller::getInstance();

  // Disconnect all zombies http, rfb, control clients though killing
  // their threads.
  zombieKiller->killAllZombies();

  m_rfbClientManager->removeListener(this);

  delete m_rfbClientManager;

  m_log.info(_T("Shutdown WinSock"));

  try {
    WindowsSocket::cleanup();
  } catch (Exception &ex) {
    m_log.error(_T("%s"), ex.getMessage());
  }
}

// Remark: this method can be called from other threads.
void TvnServer::onConfigReload(ServerConfig *serverConfig)
{
  // Start/stop/restart RFB servers if needed.
  {
    // FIXME: Protect only primitive operations.
    // FIXME: Nested lock in protected code (congifuration locking).
    AutoLock l(&m_mutex);

    bool toggleMainRfbServer =
      m_srvConfig->isAcceptingRfbConnections() != (m_rfbServer != 0);
    bool changeMainRfbPort = m_rfbServer != 0 &&
      (m_srvConfig->getRfbPort() != (int)m_rfbServer->getBindPort());

    const TCHAR *bindHost =
      m_srvConfig->isOnlyLoopbackConnectionsAllowed() ? _T("localhost") : _T("0.0.0.0");
    bool changeBindHost =  m_rfbServer != 0 &&
      _tcscmp(m_rfbServer->getBindHost(), bindHost) != 0;

    if (toggleMainRfbServer ||
        changeMainRfbPort ||
        changeBindHost) {
      restartMainRfbServer();
    }

    // NOTE: ExtraRfbServers::reload() does not throw exceptions if some
    //       servers did not start. However, it returns false in that case.
    //       Here we ignore all errors.
    (void)m_extraRfbServers.reload(m_runAsService, m_rfbClientManager);
  }

  // Start/stop/restart HTTP server if needed.
  {
    // FIXME: Protect only primitive operations.
    AutoLock l(&m_mutex);

    bool toggleHttp =
      m_srvConfig->isAcceptingHttpConnections() != (m_httpServer != 0);
    bool changePort = m_httpServer != 0 &&
      (m_srvConfig->getHttpPort() != (int)m_httpServer->getBindPort());

    if (toggleHttp || changePort) {
      restartHttpServer();
    }
  }
  changeLogProps();
}

void TvnServer::getServerInfo(TvnServerInfo *info)
{
  bool rfbServerListening = true;
  {
    AutoLock l(&m_mutex);
    rfbServerListening = m_rfbServer != 0;
  }

  StringStorage statusString;

  // Vnc authentication enabled.
  bool vncAuthEnabled = m_srvConfig->isUsingAuthentication();
  // No vnc passwords are set.
  bool noVncPasswords = !m_srvConfig->hasPrimaryPassword() && !m_srvConfig->hasReadOnlyPassword();
  // Determinates that main rfb server cannot accept connection in case of passwords problem.
  bool vncPasswordsError = vncAuthEnabled && noVncPasswords;

  if (rfbServerListening) {
    if (vncPasswordsError) {
      statusString = StringTable::getString(IDS_NO_PASSWORDS_SET);
    } else {
      // FIXME: Usage of deprecated FUNCTION!
      char localAddressString[1024];
      getLocalIPAddrString(localAddressString, 1024);
      AnsiStringStorage ansiString(localAddressString);
      ansiString.toStringStorage(&statusString);

      if (!vncAuthEnabled) {
        statusString.appendString(StringTable::getString(IDS_NO_AUTH_STATUS));
      } // if no auth enabled.
    } // accepting connections and no problem with passwords.
  } else {
    statusString = StringTable::getString(IDS_SERVER_NOT_LISTENING);
  } // not accepting connections.

  UINT stringId = m_runAsService ? IDS_TVNSERVER_SERVICE : IDS_TVNSERVER_APP;

  info->m_statusText.format(_T("%s - %s"),
                            StringTable::getString(stringId),
                            statusString.getString());
  info->m_acceptFlag = rfbServerListening && !vncPasswordsError;
  info->m_serviceFlag = m_runAsService;
}

void TvnServer::generateExternalShutdownSignal()
{
  AutoLock l(&m_listeners);

  vector<TvnServerListener *>::iterator it;
  for (it = m_listeners.begin(); it != m_listeners.end(); it++) {
    TvnServerListener *each = *it;

    each->onTvnServerShutdown();
  } // for all listeners.
}

bool TvnServer::isRunningAsService() const
{
  return m_runAsService;
}

void TvnServer::afterFirstClientConnect()
{
  if (timeBeginPeriod(m_contextSwitchResolution) == TIMERR_NOERROR) {
    m_log.message(_T("Set context switch resolution: %d ms"), m_contextSwitchResolution);
  }
  else {
    m_log.message(_T("Can't change context switch resolution to: %d ms"), m_contextSwitchResolution);
  }

}

void TvnServer::afterLastClientDisconnect()
{
  m_log.message(_T("Restore context switch resolution"));
  timeEndPeriod(m_contextSwitchResolution);

  ServerConfig::DisconnectAction action = m_srvConfig->getDisconnectAction();

  // Disconnect action must be executed in process on interactive user session to take effect.
  // Now, choose application keys for specified action.

  StringStorage keys;

  switch (action) {
  case ServerConfig::DA_LOCK_WORKSTATION:
    keys.format(_T("%s"), AdditionalActionApplication::LOCK_WORKSTATION_KEY);
    break;
  case ServerConfig::DA_LOGOUT_WORKSTATION:
    keys.format(_T("%s"), AdditionalActionApplication::LOGOUT_KEY);
    break;
  default:
    return;
  }

  Process *process;

  // Choose how to start process.
  StringStorage thisModulePath;
  Environment::getCurrentModulePath(&thisModulePath);
  thisModulePath.quoteSelf();
  if (isRunningAsService()) {
    bool connectToRdp = m_srvConfig->getConnectToRdpFlag();
    process = new CurrentConsoleProcess(&m_log, connectToRdp, thisModulePath.getString(),
                                        keys.getString());
  } else {
    process = new Process(thisModulePath.getString(), keys.getString());
  }

  m_log.message(_T("Execute disconnect action in separate process"));

  try {
    process->start();
  } catch (SystemException &ex) {
    m_log.error(_T("Failed to start application: \"%s\""), ex.getMessage());
  }

  delete process;
}

void TvnServer::restartHttpServer()
{
  // FIXME: Errors are critical here, they should not be ignored.

  stopHttpServer();

  if (m_srvConfig->isAcceptingHttpConnections()) {
    m_log.message(_T("Starting HTTP server"));
    try {
      // FIXME: HTTP server should bind to localhost if only loopback
      //        connections are allowed.
      m_httpServer = new HttpServer(_T("0.0.0.0"), m_srvConfig->getHttpPort(), m_runAsService, &m_log);
    } catch (Exception &ex) {
      m_log.error(_T("Failed to start HTTP server: \"%s\""), ex.getMessage());
    }
  }
}

void TvnServer::restartControlServer()
{
  // FIXME: Memory leaks.
  // FIXME: Errors are critical here, they should not be ignored.

  stopControlServer();

  m_log.message(_T("Starting control server"));

  try {
    StringStorage pipeName;
    ControlPipeName::createPipeName(isRunningAsService(), &pipeName, &m_log);

    // FIXME: Memory leak
    SecurityAttributes *pipeSecurity = new SecurityAttributes();
    pipeSecurity->setInheritable();
    pipeSecurity->shareToAllUsers();

    const unsigned int maxControlServerPipeBufferSize = 0x10000;
    PipeServer *pipeServer = new PipeServer(pipeName.getString(), maxControlServerPipeBufferSize, pipeSecurity);
    m_controlServer = new ControlServer(pipeServer , m_rfbClientManager, &m_log);
  } catch (Exception &ex) {
    m_log.error(_T("Failed to start control server: \"%s\""), ex.getMessage());
  }
}

void TvnServer::restartMainRfbServer()
{
  // FIXME: Errors are critical here, they should not be ignored.

  stopMainRfbServer();

  if (!m_srvConfig->isAcceptingRfbConnections()) {
    return;
  }

  const TCHAR *bindHost = m_srvConfig->isOnlyLoopbackConnectionsAllowed() ? _T("localhost") : _T("0.0.0.0");
  unsigned short bindPort = m_srvConfig->getRfbPort();

  m_log.message(_T("Starting main RFB server"));

  try {
    m_rfbServer = new RfbServer(bindHost, bindPort, m_rfbClientManager, m_runAsService, &m_log);
  } catch (Exception &ex) {
    m_log.error(_T("Failed to start main RFB server: \"%s\""), ex.getMessage());
  }
}

void TvnServer::stopHttpServer()
{
  m_log.message(_T("Stopping HTTP server"));

  HttpServer *httpServer = 0;
  {
    AutoLock l(&m_mutex);
    httpServer = m_httpServer;
    m_httpServer = 0;
  }
  if (httpServer != 0) {
    delete httpServer;
  }
}

void TvnServer::stopControlServer()
{
  m_log.message(_T("Stopping control server"));

  ControlServer *controlServer = 0;
  {
    AutoLock l(&m_mutex);
    controlServer = m_controlServer;
    m_controlServer = 0;
  }
  if (controlServer != 0) {
    delete controlServer;
  }
}

void TvnServer::stopMainRfbServer()
{
  m_log.message(_T("Stopping main RFB server"));

  RfbServer *rfbServer = 0;
  {
    AutoLock l(&m_mutex);
    rfbServer = m_rfbServer;
    m_rfbServer = 0;
  }
  if (rfbServer != 0) {
    delete rfbServer;
  }
}

void TvnServer::changeLogProps()
{
  StringStorage logDir;
  unsigned char logLevel;
  {
    AutoLock al(&m_mutex);
    m_srvConfig->getLogFileDir(&logDir);
    logLevel = m_srvConfig->getLogLevel();
  }
  m_logInitListener->onChangeLogProps(logDir.getString(), logLevel);
}
