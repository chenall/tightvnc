// Copyright (C) 2010,2011,2012 GlavSoft LLC.
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

#include "LogServer.h"
#include "thread/AutoLock.h"

LogServer::LogServer(const TCHAR *publicPipeName)
: m_listenLogServer(0),
  m_publicPipeName(publicPipeName),
  m_logLevel(0),
  m_headerLineCount(0),
  m_totalLogLines(0)
{
}

LogServer::~LogServer()
{
  if (m_listenLogServer != 0) delete m_listenLogServer;

  {
    AutoLock al(&m_logPropsMutex);
    for (ConnListIter iter = m_notAuthConnList.begin();
         iter != m_notAuthConnList.end(); iter++) {
      (*iter)->close();
    }
    for (ConnListIter iter = m_connList.begin();
         iter != m_connList.end(); iter++) {
      (*iter)->close();
    }
  }
  m_threadCollector.destroyAllThreads();
  {
    AutoLock al(&m_logPropsMutex);
    for (FAccountListIter iter = m_fileAccountList.begin();
         iter != m_fileAccountList.end(); iter++) {
      delete (*iter).second;
    }
  }
}

void LogServer::start(const TCHAR *logDir,
                      unsigned char logLevel, size_t headerLineCount)
{
  m_headerLineCount = headerLineCount;
  m_logDir.setString(logDir);
  m_logLevel = logLevel;
  m_listenLogServer = new ListenLogServer(m_publicPipeName.getString(), this);
}

void LogServer::changeLogProps(const TCHAR *newLogDir, unsigned char newLevel)
{
  AutoLock al(&m_logPropsMutex);
  m_logLevel = newLevel;
  m_logDir.setString(newLogDir);

  // Update this to the existing accounts.
  for (FAccountListIter iter = m_fileAccountList.begin();
       iter != m_fileAccountList.end(); iter++) {
    (*iter).second->changeLogProps(newLogDir, newLevel);
  }
  // Update existing auth connection
  for (ConnListIter iter = m_connList.begin();
       iter != m_connList.end(); iter++) {
    (*iter)->changeLogLevel(m_logLevel);
  }
  // Update existing not auth connection
  for (ConnListIter iter = m_notAuthConnList.begin();
       iter != m_notAuthConnList.end(); iter++) {
    (*iter)->changeLogLevel(m_logLevel);
  }
}

void LogServer::storeHeader()
{
  AutoLock al(&m_logPropsMutex);
  if (m_fileAccountList.size() >= 1) {
    m_fileAccountList[0]->storeHeader();
  }
}

void LogServer::onNewConnection(Channel *channel)
{
  AutoLock al(&m_logPropsMutex);
  m_notAuthConnList.push_back(new LogConn(channel, this, this, m_logLevel));
}

FileAccountHandle LogServer::onLogConnAuth(LogConn *logConn, bool success,
                                           const TCHAR *fileName)
{
  // All connections gives to ThreadCollector
  m_threadCollector.addThread(logConn);
  // Removing this connection from m_notAuthConnList
  {
    AutoLock al(&m_logPropsMutex);
    for (ConnListIter iter = m_notAuthConnList.begin();
         iter != m_notAuthConnList.end(); iter++) {
      LogConn *connOfList = *iter;
      if (connOfList == logConn) {
        m_notAuthConnList.erase(iter);
        break;
      }
    }
  }
  // Adding this connection to m_connList if success authentication
  if (success) {
    AutoLock al(&m_logPropsMutex);
    m_connList.push_back(logConn);
    return addConnection(fileName);
  } // Else ZombieKiller will destroy logConn.
  return 0;
}

void LogServer::onDisconnect(LogConn *logConn)
{
  AutoLock al(&m_logPropsMutex);
  // Search and removing this connection from m_notAuthConnList.
  for (ConnListIter iter = m_notAuthConnList.begin();
       iter != m_notAuthConnList.end(); iter++) {
    LogConn *connOfList = *iter;
    if (connOfList == logConn) {
      m_notAuthConnList.erase(iter);
      return;
    }
  }
  // Search and removing this connection from m_AuthConnList.
  for (ConnListIter iter = m_connList.begin();
       iter != m_connList.end(); iter++) {
    LogConn *connOfList = *iter;
    if (connOfList == logConn) {
      m_connList.erase(iter);
      break;
    }
  }
}

void LogServer::onLog(FileAccountHandle handle,
                      unsigned int processId,
                      unsigned int threadId,
                      const DateTime *dt,
                      int level,
                      const TCHAR *message)
{
  AutoLock al(&m_logPropsMutex);
  FAccountListIter iter = m_fileAccountList.find(handle);
  if (iter == m_fileAccountList.end()) {
    throw Exception(_T("Unhandled log message"));
  }
  (*iter).second->print(processId, threadId, dt, level, message);

  m_totalLogLines++;
  if (m_totalLogLines == m_headerLineCount) {
    storeHeader();
  }
}

void LogServer::onAnErrorFromLogConn(const TCHAR *message)
{
}

FileAccountHandle LogServer::addConnection(const TCHAR *fileName)
{
  AutoLock al(&m_logPropsMutex);
  for (FAccountListIter iter = m_fileAccountList.begin();
       iter != m_fileAccountList.end(); iter++) {
    if ((*iter).second->isTheOurFileName(fileName)) {
      return (*iter).first;
    }
  }
  size_t count = m_fileAccountList.size();
  bool logHeadEnabled = count == 0;
  m_fileAccountList[count] = new FileAccount(m_logDir.getString(),
                                             fileName, m_logLevel,
                                             logHeadEnabled);
  return count;
}
