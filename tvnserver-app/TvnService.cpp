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

#include "TvnService.h"

#include "ServerCommandLine.h"
#include "tvnserver-app/NamingDefs.h"

#include "win-system/SCMClient.h"
#include "win-system/Environment.h"

const TCHAR TvnService::SERVICE_COMMAND_LINE_KEY[] = _T("-service");

TvnService::TvnService(WinServiceEvents *winServiceEvents,
                       NewConnectionEvents *newConnectionEvents)
: Service(ServiceNames::SERVICE_NAME), m_tvnServer(0),
  m_winServiceEvents(winServiceEvents),
  m_newConnectionEvents(newConnectionEvents),
  m_logServer(LogNames::LOG_PIPE_PUBLIC_NAME),
  m_clientLogger(LogNames::LOG_PIPE_PUBLIC_NAME, LogNames::SERVER_LOG_FILE_STUB_NAME)
{
}

TvnService::~TvnService()
{
}

void TvnService::onStart()
{
  try {
    m_winServiceEvents->enable();
    // FIXME: Use real logger instead of zero.
    m_tvnServer = new TvnServer(true, m_newConnectionEvents, this, &m_clientLogger);
    m_tvnServer->addListener(this);
    m_winServiceEvents->onSuccServiceStart();
  } catch (Exception &e) {
    m_winServiceEvents->onFailedServiceStart(&StringStorage(e.getMessage()));
  }
}

void TvnService::main()
{
  m_shutdownEvent.waitForEvent();
  m_tvnServer->removeListener(this);
  delete m_tvnServer;
  m_tvnServer = 0;

  m_winServiceEvents->onServiceStop();
}

void TvnService::onStop()
{
  m_shutdownEvent.notify();
}

void TvnService::onTvnServerShutdown()
{
  TvnService::stop();
}

void TvnService::install()
{
  StringStorage binPath;

  TvnService::getBinPath(&binPath);

  SCMClient scManager;

  scManager.installService(ServiceNames::SERVICE_NAME,
                           ServiceNames::SERVICE_NAME_TO_DISPLAY,
                           binPath.getString(), _T(""));
}

void TvnService::remove()
{
  SCMClient scManager;

  scManager.removeService(ServiceNames::SERVICE_NAME);
}

void TvnService::reinstall()
{
  try {
    remove();
  } catch (...) { }

  TvnService::install();
}

void TvnService::start(bool waitCompletion)
{
  SCMClient scManager;

  scManager.startService(ServiceNames::SERVICE_NAME, waitCompletion);
}

void TvnService::stop(bool waitCompletion)
{
  SCMClient scManager;

  scManager.stopService(ServiceNames::SERVICE_NAME, waitCompletion);
}

bool TvnService::getBinPath(StringStorage *binPath)
{
  StringStorage pathToServiceBinary;

  // Get executable folder first.
  if (!Environment::getCurrentModulePath(&pathToServiceBinary)) {
    return false;
  }

  // Create formatted binary path.
  binPath->format(_T("\"%s\" %s"),
                  pathToServiceBinary.getString(),
                  SERVICE_COMMAND_LINE_KEY);

  return true;
}

void TvnService::onLogInit(const TCHAR *logDir, const TCHAR *fileName,
                           unsigned char logLevel)
{
  size_t headerLineCount = m_clientLogger.getLogDumpSize();
  m_logServer.start(logDir, logLevel, headerLineCount);
  m_clientLogger.connect();
}

void TvnService::onChangeLogProps(const TCHAR *newLogDir, unsigned char newLevel)
{
  m_logServer.changeLogProps(newLogDir, newLevel);
}
