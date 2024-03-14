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
#include "Environment.h"
#include "PipeImpersonatedThread.h"
#include <crtdbg.h>


DynamicLibrary *WTS::m_kernel32Library = 0;
DynamicLibrary *WTS::m_wtsapi32Library = 0;
pWTSGetActiveConsoleSessionId WTS::m_WTSGetActiveConsoleSessionId = 0;
pWTSQueryUserToken WTS::m_WTSQueryUserToken = 0;
pWTSQuerySessionInformation WTS::m_WTSQuerySessionInformation = 0;
pWTSEnumerateSessions WTS::m_WTSEnumerateSessions = 0;
pWTSFreeMemory WTS::m_WTSFreeMemory = 0;

volatile bool WTS::m_initialized = false;

HANDLE WTS::m_userProcessToken = INVALID_HANDLE_VALUE;

LocalMutex WTS::m_mutex;

DWORD WTS::getActiveConsoleSessionId(LogWriter *log)
{
  AutoLock l(&m_mutex);
  DWORD id;

  if (!m_initialized) {
    initialize(log);
  }

  if (m_WTSGetActiveConsoleSessionId == 0) {
    return 0;
  }
  id = m_WTSGetActiveConsoleSessionId();

  log->debug(_T("Active console session Id: %d"), id);

  return id;
}

DWORD WTS::getRdpSessionId(LogWriter *log)
{
  AutoLock l(&m_mutex);

  if (!m_initialized) {
    initialize(log);
  }

  if (m_WTSEnumerateSessions == 0) {
    return 0;
  }
  
  PWTS_SESSION_INFO sessionInfo = 0;
  DWORD count = 0;
  DWORD sessionId = 0;

  if (m_WTSEnumerateSessions(WTS_CURRENT_SERVER_HANDLE, 0, 1, &sessionInfo, &count)) {
    for (DWORD i = 0; i < count; i++) {
      if (sessionInfo[i].State == WTSActive) {
        StringStorage sessionName(sessionInfo[i].pWinStationName);
        log->debug(_T("Enumerate Sessions, Id: %d, Name: %s"), sessionInfo[i].SessionId, sessionName.getString());
        sessionName.toLowerCase();
        if (sessionName.find(_T("rdp")) != 0) {
          sessionId = (DWORD)sessionInfo[i].SessionId;
          log->debug(_T("RDP Session selected, Id: %d"), sessionId);
        }
      }
    }
    wtsFreeMemory(sessionInfo);
  }
  return sessionId;
}


bool WTS::SessionIsRdpSession(DWORD sessionId, LogWriter *log)
{
  {
    AutoLock l(&m_mutex);

    if (!m_initialized) {
      initialize(log);
    }
  }
  bool res = false;
  if (m_WTSQuerySessionInformation == 0) {
    return res;
  }

  LPWSTR *buffer;
  DWORD byteCount(0);

  if (m_WTSQuerySessionInformation(WTS_CURRENT_SERVER_HANDLE, sessionId,
    WTSWinStationName, &buffer, &byteCount) == 0) {
    return res;
  }
  StringStorage sessionName((TCHAR *)buffer);
  sessionName.toLowerCase();
  if (sessionName.find(_T("rdp")) != 0) {
    res = true;
  }
  wtsFreeMemory(buffer);
  return res;
}


HANDLE WTS::queryConsoleUserToken(LogWriter *log) throw(SystemException)
{
  DWORD sessionId = getActiveConsoleSessionId(log);
  return sessionUserToken(sessionId, log);
}

HANDLE WTS::sessionUserToken(DWORD sessionId, LogWriter* log)
{
  HANDLE token = NULL;
  {
    AutoLock l(&m_mutex);

    if (!m_initialized) {
      initialize(log);
    }
  }

  AutoLock l(&m_mutex);

  if (m_WTSQueryUserToken != 0) {
    if (!m_WTSQueryUserToken(sessionId, &token)) {
      throw SystemException(_T("WTSQueryUserToken error:"));
    }
  }
  else {
    if (m_userProcessToken == INVALID_HANDLE_VALUE) {
      throw SystemException(_T("No console user process id specified"));
    }
    if (!DuplicateTokenEx(m_userProcessToken, 0, NULL, SecurityImpersonation,
      TokenPrimary, &token)) {
      throw SystemException(_T("Could not duplicate token"));
    }
  }
  return token;
}


StringStorage WTS::getCurrentUserName(LogWriter *log)
{

  DWORD sessionId = getActiveConsoleSessionId(log);
  return getUserName(sessionId, log);
}

StringStorage WTS::getUserName(DWORD sessionId, LogWriter* log)
{
  StringStorage userName;
  if (m_WTSQuerySessionInformation == 0) {
    return userName;
  }

  LPTSTR* buffer;
  DWORD byteCount;
  if (m_WTSQuerySessionInformation(WTS_CURRENT_SERVER_HANDLE, sessionId,
    WTSUserName, &buffer, &byteCount) != 0) {
    userName.setString((TCHAR*)buffer);
    wtsFreeMemory(buffer);
  }
  return userName;
}

bool WTS::sessionIsLocked(DWORD sessionId, LogWriter* log)
{
#ifndef UNICODE
  return false;
#endif

  if (m_WTSQuerySessionInformation == 0) {
    return false;
  }

  PWTSINFOEXW buffer = 0;
  DWORD byteCount;
  if (m_WTSQuerySessionInformation(WTS_CURRENT_SERVER_HANDLE, sessionId,
    WTSSessionInfoEx, (LPWSTR**)&buffer, &byteCount) == 0) {
    return false;
  }
  if (buffer->Level != 1) {
    wtsFreeMemory(buffer);
    return false;
  }
  WTSINFOEX_LEVEL1_W info = buffer->Data.WTSInfoExLevel1;
  LONG locked = info.SessionFlags;
  wtsFreeMemory(buffer);
  // reverse for Windows Server 2008 R2 and Windows 7
  if (Environment::isWin7()) {
    if (locked == WTS_SESSIONSTATE_UNLOCK) {
      return true;
    } 
	  return false;
  }
  if (locked == WTS_SESSIONSTATE_LOCK) {
    return true;
  }
  return false;
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
    m_WTSEnumerateSessions = (pWTSEnumerateSessions)m_wtsapi32Library->getProcAddress("WTSEnumerateSessionsW");
#else
    m_WTSQuerySessionInformation = (pWTSQuerySessionInformation)m_wtsapi32Library->getProcAddress("WTSQuerySessionInformationA");
    m_WTSEnumerateSessions = (pWTSEnumerateSessions)m_wtsapi32Library->getProcAddress("WTSEnumerateSessionsA");
#endif
    m_WTSFreeMemory = (pWTSFreeMemory)m_wtsapi32Library->getProcAddress("WTSFreeMemory");
  } catch (Exception &e) {
    log->error(_T("Can't load the Wtsapi32.dll library: %s"), e.getMessage());
  }

  m_initialized = true;
}

HANDLE currentProcessUserToken(LogWriter* log)
{
  HANDLE token = NULL;
  HANDLE procHandle = GetCurrentProcess();
  log->debug(_T("Try OpenProcessToken(%p, , )"), (void*)procHandle);
  if (OpenProcessToken(procHandle, TOKEN_DUPLICATE, &token) == 0) {
    throw SystemException();
  }
  return token;
}


HANDLE WTS::duplicateCurrentProcessUserToken(bool rdpEnabled, LogWriter* log)
{
  DWORD rdpSession = 0;
  DWORD activeSession = 0;
  DWORD sessionId = 0;
  bool rdp = false;

  if (rdpEnabled) {
    rdpSession = getRdpSessionId(log);
    if (rdpSession) {
      rdp = true;
    }
  }
  activeSession = getActiveConsoleSessionId(log);

  log->debug(_T("rdpSession user name: %s"), getUserName(rdpSession, log).getString());
  log->debug(_T("activeSession user name: %s"), getUserName(activeSession, log).getString());

  if (rdp) {
    sessionId = rdpSession;
    log->info(_T("Connect as RDP user at %d session"), sessionId);
  } else {
    sessionId = getActiveConsoleSessionId(log);
    log->info(_T("Connect as current user at %d session"), sessionId);
  }
  log->debug(_T("Session user name: %s"), getUserName(sessionId, log).getString());

  HANDLE token;

  if (rdp && !sessionIsLocked(sessionId, log)) {
    token = sessionUserToken(sessionId, log);
  } else {
    token = currentProcessUserToken(log);
  }

  HANDLE userToken = duplicateUserImpersonationToken(token, sessionId, log);
  CloseHandle(token);

  return userToken;
}

HANDLE WTS::duplicateUserImpersonationToken(HANDLE token, DWORD sessionId, LogWriter* log)
{
  HANDLE userToken;

  log->debug(_T("Try DuplicateTokenEx(%p, , , , , )"), (void*)token);
  if (DuplicateTokenEx(token,
    MAXIMUM_ALLOWED,
    0,
    SecurityImpersonation,
    TokenPrimary,
    &userToken) == 0) {
    throw SystemException();
  }

  log->debug(_T("Try SetTokenInformation(%p, , , )"), (void*)userToken);
  if (SetTokenInformation(userToken,
    (TOKEN_INFORMATION_CLASS)TokenSessionId,
    &sessionId,
    sizeof(sessionId)) == 0) {
    throw SystemException();
  }
  // Fix Windows 8/8.1 UIAccess restrictions (Alt-Tab) for server as service
  // http://stackoverflow.com/questions/13972165/pressing-winx-alt-tab-programatically
  // For application we need to set /uiAccess='true' in linker manifest, sign binary 
  // and run from "Program Files/"

  DWORD uiAccess = 1; // Nonzero enables UI control
  log->debug(_T("Try SetTokenInformation(%p, , , ) with UIAccess=1"), (void*)userToken);

  if (SetTokenInformation(userToken,
    (TOKEN_INFORMATION_CLASS)TokenUIAccess,
    &uiAccess,
    sizeof(uiAccess)) == 0) {
    log->info(_T("Can't set UIAccess=1, ignore it"));
  }
  StringStorage name = getTokenUserName(userToken);
  log->debug(_T("duplicate user token for user: %s, session ID: %d"), name.getString(), sessionId);

  return userToken;
}

StringStorage WTS::getTokenUserName(HANDLE token) {
  StringStorage name;
  DWORD tokenSize = 0;
  GetTokenInformation(token, TokenUser, NULL, 0, &tokenSize);

  if (tokenSize <= 0) {
    return name;
  }

  BYTE* data = new BYTE[tokenSize];
  GetTokenInformation(token, TokenUser, data, tokenSize, &tokenSize); // 3- GetTokenInformation
  TOKEN_USER* pUser = (TOKEN_USER*)data;
  PSID pSID = pUser->User.Sid;
  DWORD userSize = 0;
  DWORD domainSize = 0;
  SID_NAME_USE sidName;
  LookupAccountSid(NULL, pSID, NULL, &userSize, NULL, &domainSize, &sidName);
  TCHAR* user = new TCHAR[userSize + 1];
  TCHAR* domain = new TCHAR[domainSize + 1];
  LookupAccountSid(NULL, pSID, user, &userSize, domain, &domainSize, &sidName); 
  user[userSize] = _T('\0');
  name.setString(user);
  delete[] data;
  delete[] domain;
  delete[] user;
  return name;
}

WTS::WTS()
{
}
