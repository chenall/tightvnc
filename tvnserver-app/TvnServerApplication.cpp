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

#include "TvnServerApplication.h"
#include "ServerCommandLine.h"
#include "TvnServerHelp.h"

#include "thread/GlobalMutex.h"

#include "util/ResourceLoader.h"
#include "util/StringTable.h"
#include "tvnserver-app/NamingDefs.h"
#include "win-system/WinCommandLineArgs.h"

#include "tvnserver/resource.h"

TvnServerApplication::TvnServerApplication(HINSTANCE hInstance,
                                           const TCHAR *windowClassName,
                                           const TCHAR *commandLine,
                                           NewConnectionEvents *newConnectionEvents)
: WindowsApplication(hInstance, windowClassName),
  m_fileLogger(true),
  m_tvnServer(0),
  m_commandLine(commandLine),
  m_newConnectionEvents(newConnectionEvents)
{
}

TvnServerApplication::~TvnServerApplication()
{
}

int TvnServerApplication::run()
{
  // FIXME: May be an unhandled exception.
  // Check wrong command line and situation when we need to show help.

  try {
    ServerCommandLine parser;
    WinCommandLineArgs cmdArgs(m_commandLine.getString());
    if (!parser.parse(&cmdArgs) || parser.showHelp()) {
      throw Exception(_T("Wrong command line argument"));
    }
  } catch (...) {
    TvnServerHelp::showUsage();
    return 0;
  }

  // Reject 2 instances of TightVNC server application.

  GlobalMutex *appInstanceMutex;

  try {
    appInstanceMutex = new GlobalMutex(
      ServerApplicationNames::SERVER_INSTANCE_MUTEX_NAME, false, true);
  } catch (...) {
    MessageBox(0,
               StringTable::getString(IDS_SERVER_ALREADY_RUNNING),
               StringTable::getString(IDS_MBC_TVNSERVER), MB_OK | MB_ICONEXCLAMATION);
    return 1;
  }

  // Start TightVNC server and TightVNC control application.
  try {
    m_tvnServer = new TvnServer(false, m_newConnectionEvents, this, &m_fileLogger);
    m_tvnServer->addListener(this);
    m_tvnControlRunner = new WsConfigRunner(&m_fileLogger);

    int exitCode = WindowsApplication::run();

    delete m_tvnControlRunner;
    m_tvnServer->removeListener(this);
    delete m_tvnServer;
    delete appInstanceMutex;
    return exitCode;
  } catch (Exception &e) {
    // FIXME: Move string to resource
    StringStorage message;
    message.format(_T("Couldn't run the server: %s"), e.getMessage());
    MessageBox(0,
               message.getString(),
               _T("Server error"), MB_OK | MB_ICONEXCLAMATION);
    return 1;
  }
}

void TvnServerApplication::onTvnServerShutdown()
{
  WindowsApplication::shutdown();
}

void TvnServerApplication::onLogInit(const TCHAR *logDir, const TCHAR *fileName,
                                     unsigned char logLevel)
{
  m_fileLogger.init(logDir, fileName, logLevel);
  m_fileLogger.storeHeader();
}

void TvnServerApplication::onChangeLogProps(const TCHAR *newLogDir, unsigned char newLevel)
{
  m_fileLogger.changeLogProps(newLogDir, newLevel);
}
