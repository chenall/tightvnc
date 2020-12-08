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

#include "DesktopServerApplication.h"
#include "DesktopServerCommandLine.h"
#include "util/ResourceLoader.h"
#include "desktop/WallpaperUtil.h"
#include "win-system/WTS.h"
#include "win-system/Environment.h"
#include "win-system/SharedMemory.h"
#include "tvnserver-app/NamingDefs.h"
#include "TimeAPI.h"

DesktopServerApplication::DesktopServerApplication(HINSTANCE appInstance,
                                                   const TCHAR *windowClassName,
                                                   const CommandLineArgs *cmdArgs)
: LocalWindowsApplication(appInstance, windowClassName),
  m_clToSrvChan(0),
  m_srvToClChan(0),
  m_clToSrvGate(0),
  m_srvToClGate(0),
  m_dispatcher(0),
  m_updHandlerSrv(0),
  m_uiSrv(0),
  m_cfgServer(0),
  m_gateKickHandler(0),
  m_sessionChangesWatcher(0),
  m_configurator(true),
  m_clientLogger(LogNames::LOG_PIPE_PUBLIC_NAME,
                LogNames::SERVER_LOG_FILE_STUB_NAME),
  m_contextSwitchResolution(1),
  m_log(&m_clientLogger)
{
  try {
    m_clientLogger.connect();
  } catch (...) {
  }

  DesktopServerCommandLine cmdLineParser;

  cmdLineParser.parse(cmdArgs);

  // Keep session id
  DWORD baseSessionId = WTS::getActiveConsoleSessionId(&m_log);
  Configurator::getInstance()->addListener(this);

  try {
    // Transport initialization
    // Get pipe channel handles by the shared memory
    StringStorage shMemName;
    cmdLineParser.getSharedMemName(&shMemName);
    SharedMemory shMem(shMemName.getString(), 72);
    UINT64 *mem = (UINT64 *)shMem.getMemPointer();

    DateTime startTime = DateTime::now();

    WindowsEvent m_sleepInterval;
    while (mem[0] == 0) {
      unsigned int timeForWait = max((int)10000 - 
                                     (int)(DateTime::now() -
                                           startTime).getTime(),
                                     0);
      if (timeForWait == 0) {
        throw Exception(_T("The desktop server time out expired"));
      }
      m_sleepInterval.waitForEvent(10);
    }

    HANDLE readPipeHandle, writePipeHandle;
    unsigned int maxPortionSize;

    readPipeHandle = (HANDLE)mem[1];
    writePipeHandle = (HANDLE)mem[2];
    maxPortionSize = (unsigned int)mem[3];
    m_clToSrvChan = new AnonymousPipe(readPipeHandle, writePipeHandle, maxPortionSize, &m_log);
    m_log.info(_T("Client->server readPipeHandle = %p, writePipeHandle = %p"), readPipeHandle, writePipeHandle);

    readPipeHandle = (HANDLE)mem[4];
    writePipeHandle = (HANDLE)mem[5];
    maxPortionSize = (unsigned int)mem[6];
    m_srvToClChan = new AnonymousPipe(readPipeHandle, writePipeHandle, maxPortionSize, &m_log);
    m_log.info(_T("Server->client readPipeHandle = %p, writePipeHandle = %p"), readPipeHandle, writePipeHandle);

    m_clToSrvGate = new BlockingGate(m_clToSrvChan);
    m_srvToClGate = new BlockingGate(m_srvToClChan);

    // Server initializations
    m_dispatcher = new DesktopSrvDispatcher(m_clToSrvGate, this, &m_log);

    m_updHandlerSrv = new UpdateHandlerServer(m_srvToClGate, m_dispatcher, this, &m_log);
    m_uiSrv = new UserInputServer(m_srvToClGate, m_dispatcher, this, &m_log);
    m_cfgServer = new ConfigServer(m_dispatcher, &m_log);
    m_gateKickHandler = new GateKickHandler(m_dispatcher);

    // Start servers
    m_dispatcher->resume();

    // Spy for the session change.
    m_sessionChangesWatcher = new SessionChangesWatcher(this, &m_log);
  } catch (Exception &e) {
    m_log.error(_T("Desktop server application failed with error: %s"),e.getMessage());
    freeResources();
    throw;
  }
}

DesktopServerApplication::~DesktopServerApplication()
{
  m_log.info(_T("The Desktop server application destructor has been called"));
  freeResources();
  m_log.info(_T("Desktop server application has been terminated"));
}

void DesktopServerApplication::freeResources()
{
  try {
    if (m_clToSrvChan) m_clToSrvChan->close();
  } catch (Exception &e) {
    m_log.error(_T("Cannot close client->server channel: %s"),
               e.getMessage());
  }
  try {
    if (m_srvToClChan) m_srvToClChan->close();
  } catch (Exception &e) {
    m_log.error(_T("Cannot close server->client channel: %s"),
               e.getMessage());
  }

  if (m_sessionChangesWatcher) delete m_sessionChangesWatcher;

  // This will stop and destroy the dispatcher. So all handles will be
  // unregistered automatically.
  if (m_dispatcher) delete m_dispatcher;

  if (m_gateKickHandler) delete m_gateKickHandler;
  if (m_cfgServer) delete m_cfgServer;
  if (m_uiSrv) delete m_uiSrv;
  if (m_updHandlerSrv) delete m_updHandlerSrv;

  if (m_srvToClGate) delete m_srvToClGate;
  if (m_clToSrvGate) delete m_clToSrvGate;
  if (m_srvToClChan) delete m_srvToClChan;
  if (m_clToSrvChan) delete m_clToSrvChan;
}

void DesktopServerApplication::onAnObjectEvent()
{
  m_log.error(_T("An error has been occurred in the desktop server.")
             _T(" Application will be closed."));
  WindowsApplication::shutdown();
}

void DesktopServerApplication::onConfigReload(ServerConfig *serverConfig)
{
}

int DesktopServerApplication::run()
{
  try {
    WallpaperUtil wp(&m_log);

    int retCode = WindowsApplication::run();
    m_log.info(_T("Desktop server terminated with return code = %d"), retCode);
    return retCode;
  } catch (Exception &e) {
    m_log.error(_T("Desktop server has been terminated with error: %s"),
               e.getMessage());
    return 0;
  }
}
