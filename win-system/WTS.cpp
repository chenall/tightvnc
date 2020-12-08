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

#include "WTS.h"

#include "SystemException.h"
#include "thread/AutoLock.h"
#include "PipeImpersonatedThread.h"
#include <crtdbg.h>

DynamicLibrary *WTS::m_kernel32Library = 0;
DynamicLibrary *WTS::m_wtsapi32Library = 0;
pWTSGetActiveConsoleSessionId WTS::m_WTSGetActiveConsoleSessionId = 0;
pWTSQueryUserToken WTS::m_WTSQueryUserToken = 0;
pWTSQuerySessionInformation WTS::m_WTSQuerySessionInformation = 0;
pWTSFreeMemory WTS::m_WTSFreeMemory = 0;

volatile bool WTS::m_initialized = false;

HANDLE WTS::m_userProcessToken = INVALID_HANDLE_VALUE;

LocalMutex WTS::m_mutex;

DWORD WTS::getActiveConsoleSessionId(LogWriter *log)
{
  AutoLock l(&m_mutex);

  if (!m_initialized) {
    initialize(log);
  }

  if (m_WTSGetActiveConsoleSessionId == 0) {
    return 0;
  }

  return m_WTSGetActiveConsoleSessionId();
}

void WTS::queryConsoleUserToken(HANDLE *token, LogWriter *log) throw(SystemException)
{
  {
    AutoLock l(&m_mutex);

    if (!m_initialized) {
      initialize(log);
    }
  }

  DWORD sessionId = getActiveConsoleSessionId(log);

  AutoLock l(&m_mutex);

  if (m_WTSQueryUserToken != 0) {
    if (!m_WTSQueryUserToken(sessionId, token)) {
	  throw SystemException(_T("WTSQueryUserToken error:"));
    }
  } else {
    if (m_userProcessToken == INVALID_HANDLE_VALUE) {
      throw SystemException(_T("No console user process id specified"));
    }
    if (!DuplicateTokenEx(m_userProcessToken, 0, NULL, SecurityImpersonation,
                          TokenPrimary, token)) {
      throw SystemException(_T("Could not duplicate token"));
    }
  }
}

bool WTS::getCurrentUserName(StringStorage *userName, LogWriter *log)
{
  if (m_WTSQuerySessionInformation == 0) {
    return false;
  }

  LPTSTR *buffer;
  DWORD byteCount;
  DWORD sessionId = getActiveConsoleSessionId(log);
  if (m_WTSQuerySessionInformation(WTS_CURRENT_SERVER_HANDLE, sessionId,
                                   WTSUserName, &buffer, &byteCount) == 0) {
    return false;
  }
  userName->setString((TCHAR *)buffer);
  wtsFreeMemory(buffer);

  return true;
}

void WTS::wtsFreeMemory(void *buffer)
{
  m_WTSFreeMemory(buffer);
}

void WTS::defineConsoleUserProcessId(DWORD userProcessId)
{
  HANDLE procHandle = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, userProcessId);

  if (procHandle == 0) {
    throw SystemException();
  }

  HANDLE userProcessToken;

  if (!OpenProcessToken(procHandle, TOKEN_ALL_ACCESS, &userProcessToken)) {
    CloseHandle(procHandle);
    throw SystemException();
  }

  CloseHandle(procHandle);

  AutoLock l(&m_mutex);

  if (m_userProcessToken != 0) {
    CloseHandle(m_userProcessToken);
  }

  m_userProcessToken = userProcessToken;
}

void WTS::duplicatePipeClientToken(HANDLE pipeHandle)
{
  PipeImpersonatedThread impThread(pipeHandle);
  impThread.resume();
  impThread.waitUntilImpersonated();
  if (!impThread.getImpersonationSuccess()) {
    StringStorage faultReason, errMessage;
    impThread.getFaultReason(&faultReason);
    errMessage.format(_T("Can't impersonate thread by pipe handle: %s"),
                      faultReason.getString());
    throw Exception(errMessage.getString());
  }

  HANDLE threadHandle = OpenThread(THREAD_QUERY_INFORMATION, FALSE,
                                   impThread.getThreadId());
  if (threadHandle == 0) {
    throw SystemException(_T("Can't open thread to duplicate")
                          _T(" impersonate token"));
  }
  try {
    HANDLE userThreadToken;
    if (OpenThreadToken(threadHandle, TOKEN_ALL_ACCESS, TRUE, &userThreadToken) == 0) {
      throw SystemException(_T("Can't open process token to duplicate")
                            _T(" impersonate token"));
    }
    try {
      HANDLE userThreadDuplicatedToken;
      if (DuplicateTokenEx(userThreadToken, 0, 0, SecurityImpersonation,
                           TokenPrimary, &userThreadDuplicatedToken) == 0) {
        throw SystemException(_T("Can't duplicate token from impersonated")
                              _T(" to a named pipe client token"));
      }
      if (m_userProcessToken != INVALID_HANDLE_VALUE) {
        CloseHandle(m_userProcessToken);
      }
      m_userProcessToken = userThreadDuplicatedToken;
    } catch (...) {
      CloseHandle(userThreadToken);
      throw;
    }
    CloseHandle(userThreadToken);
  } catch (...) {
    CloseHandle(threadHandle);
    throw;
  }
  CloseHandle(threadHandle);
}

void WTS::initialize(LogWriter *log)
{
  _ASSERT(!m_initialized);

  try {
    m_kernel32Library = new DynamicLibrary(_T("Kernel32.dll"));
    m_WTSGetActiveConsoleSessionId = (pWTSGetActiveConsoleSessionId)m_kernel32Library->getProcAddress("WTSGetActiveConsoleSessionId");
  } catch (Exception &e) {
    log->error(_T("Can't load the Kernel32.dll library: %s"), e.getMessage());
  }
  try {
    m_wtsapi32Library = new DynamicLibrary(_T("Wtsapi32.dll"));
    m_WTSQueryUserToken = (pWTSQueryUserToken)m_wtsapi32Library->getProcAddress("WTSQueryUserToken");
#ifdef UNICODE
    m_WTSQuerySessionInformation = (pWTSQuerySessionInformation)m_wtsapi32Library->getProcAddress("WTSQuerySessionInformationW");
#else
    m_WTSQuerySessionInformation = (pWTSQuerySessionInformation)m_wtsapi32Library->getProcAddress("WTSQuerySessionInformationA");
#endif
    m_WTSFreeMemory = (pWTSFreeMemory)m_wtsapi32Library->getProcAddress("WTSFreeMemory");
  } catch (Exception &e) {
    log->error(_T("Can't load the Wtsapi32.dll library: %s"), e.getMessage());
  }

  m_initialized = true;
}

WTS::WTS()
{
}
