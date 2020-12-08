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

#ifndef _VIEWER_CONFIG_H_
#define _VIEWER_CONFIG_H_

#include "util/StringStorage.h"
#include "util/Singleton.h"
#include "config-lib/ConnectionHistory.h"
#include "log-writer/FileLogger.h"
#include "thread/LocalMutex.h"
#include "thread/AutoLock.h"

#include "ConnectionConfig.h"

//
// Contains base set of viewer configuration options.
//

class ViewerConfig : public Singleton<ViewerConfig>
{
public:
  ViewerConfig(const TCHAR registryPath[]);
  ~ViewerConfig();

  // Deserializes client configuration from settings storage.
  // Returns true if was no errors during deserialization,
  // false, otherwise.
  bool loadFromStorage(SettingsManager *storage);

  // Serializes client configuration from settings storage.
  // Returns true if was no errors during serialization,
  // false, otherwise.
  bool saveToStorage(SettingsManager *storage) const;

  // Sets port for incoming connection when client runs in daemoon mode
  void setListenPort(int listenPort);
  // Returns listen port value
  int getListenPort() const;

  // Changes log level in 0 - 9 range
  void setLogLevel(int logLevel);
  // Returns log level
  int getLogLevel() const;

  // Puts log directory to the logDir argument.
  void getLogDir(StringStorage *logDir) const;
  // Changes log directory 
  void setLogDir(StringStorage &logDir);

  // Creates path to log file and place value to m_pathToLogFile member
  // creates logger and return pointer to him
  Logger *initLog(const TCHAR logDir[], const TCHAR logName[], bool useSpecialFolder = true);

  // function return pointer to logger
  Logger *getLogger();

  // Sets number of connections to remember
  void setHistoryLimit(int historyLimit);
  // Returns number of connections to remember
  int getHistoryLimit() const;

  // Sets "show toolbar" flag
  void showToolbar(bool show);
  // Returns "show toolbar" flag
  bool isToolbarShown() const;

  // Sets "prompt on fullscreen flag"
  void promptOnFullscreen(bool promt);
  // Returns "prompt on fullscreen flag"
  bool isPromptOnFullscreenEnabled() const;

  // Returns path to log file if file is avaliable to write,
  // returns NULL otherwise
  const TCHAR *getPathToLogFile() const;

  // Returns connection history
  ConnectionHistory *getConnectionHistory();

protected:
  // TCP port for accepting incoming connection
  // when client runs in daemon mode
  int m_listenPort;
  // Current level of logging
  int m_logLevel;
  // Number of connections to remember
  int m_historyLimit;
  // If set then toolbar is shown, otherwise not shown
  bool m_showToolbar;
  // If set then app must show promt dialog when viewer window
  // become fullscreen
  bool m_promptOnFullscreen;
  // Log file
  StringStorage m_pathToLogFile;
  StringStorage m_logName;
  FileLogger *m_logger;
  // Connection history
  RegistryKey m_conHistoryKey; // Used by m_conHistory
  ConnectionHistory m_conHistory;
private:
  // Critical section for synchronization
  mutable LocalMutex m_cs;
};

#endif
