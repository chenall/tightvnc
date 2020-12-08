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

#include "WinEventLog.h"
#include "util/CommonHeader.h"
#include "tvnserver-app/NamingDefs.h"
#include "util/Exception.h"
#include "win-system/Environment.h"
#include "win-system/RegistryKey.h"
#include "thread/AutoLock.h"

WinEventLog::WinEventLog(LogWriter *log)
: m_hEventLog(0),
  m_log(log)
{
}

WinEventLog::~WinEventLog()
{
  deRegisterEventSource();
}

void WinEventLog::enable()
{
  deRegisterEventSource();
  try {
    updateEventSourcesSubkey();
  } catch (Exception &e) {
    m_log->error(_T("Cannot update event sources registry subkey: %s"),
               e.getMessage());
  }
  registerEventSource();
}

void WinEventLog::registerEventSource()
{
  AutoLock al(&m_hEventLogMutex);
  m_hEventLog = RegisterEventSource(0, LogNames::WIN_EVENT_PROVIDER_NAME);
}

void WinEventLog::deRegisterEventSource()
{
  AutoLock al(&m_hEventLogMutex);
  if (m_hEventLog)
    DeregisterEventSource(m_hEventLog);
  m_hEventLog = 0;
}

void WinEventLog::updateEventSourcesSubkey()
{
  StringStorage path;
  if (Environment::getCurrentModulePath(&path)) {
    StringStorage entry(_T("SYSTEM\\CurrentControlSet\\")
                        _T("services\\eventlog\\Application\\"));
    entry += LogNames::WIN_EVENT_PROVIDER_NAME;
    RegistryKey regKey(HKEY_LOCAL_MACHINE, entry.getString());
    regKey.setValueAsInt32(_T("CategoryCount"), 0);
    regKey.setValueAsString(_T("CategoryMessageFile"), path.getString());
    regKey.setValueAsString(_T("EventMessageFile"), path.getString());
    regKey.setValueAsString(_T("ParameterMessageFile"), path.getString());
    regKey.setValueAsInt32(_T("TypesSupported"),
                           EVENTLOG_ERROR_TYPE | EVENTLOG_INFORMATION_TYPE |
                           EVENTLOG_WARNING_TYPE);
  }
}

void WinEventLog::reportInfo(unsigned int messageId, const TCHAR *fmt, ...)
{
  va_list vl;
  va_start(vl, fmt);
  reportEvent(messageId, EVENTLOG_INFORMATION_TYPE, fmt, vl);
  va_end(vl);
}

void WinEventLog::reportWarning(unsigned int messageId, const TCHAR *fmt, ...)
{
  va_list vl;
  va_start(vl, fmt);
  reportEvent(messageId, EVENTLOG_WARNING_TYPE, fmt, vl);
  va_end(vl);
}

void WinEventLog::reportError(unsigned int messageId, const TCHAR *fmt, ...)
{
  va_list vl;
  va_start(vl, fmt);
  reportEvent(messageId, EVENTLOG_ERROR_TYPE, fmt, vl);
  va_end(vl);
}

#pragma warning(push)
#pragma warning(disable:4996)

void WinEventLog::reportEvent(unsigned int messageId,
                            WORD eventType,
                            const TCHAR *fmt,
                            va_list argList)
{
  HANDLE hEventLog = getLogHandle();
  if (hEventLog == 0) {
    return;
  }

  // Format the original string.
  int count = _vsctprintf(fmt, argList);
  _ASSERT(count >= 0);
  std::vector<TCHAR> formattedStringBuff(count + 1);
  TCHAR *formattedString = &formattedStringBuff.front();
  _vstprintf(formattedString, fmt, argList);

  if (ReportEvent(hEventLog,
                  eventType,
                  0, // category
                  messageId,
                  0, // pointer to an User Sid
                  1, // always only one string
                  0, // data size
                  (LPCTSTR *)&formattedString,
                  0 // data
                  ) == 0) {
    StringStorage errStr;
    Environment::getErrStr(_T("Cannot report an event to the system log"),
                           &errStr);
    m_log->error(_T("%s"), errStr.getString());
  }
}

#pragma warning(pop)

HANDLE WinEventLog::getLogHandle()
{
  AutoLock al(&m_hEventLogMutex);
  return m_hEventLog;
}
