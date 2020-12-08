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

#ifndef _WTS_H_
#define _WTS_H_

#include "util/winhdr.h"
#include "thread/LocalMutex.h"
#include "DynamicLibrary.h"
#include "SystemException.h"
#include "log-writer/LogWriter.h"

typedef DWORD (WINAPI *pWTSGetActiveConsoleSessionId)(void);
typedef BOOL (WINAPI *pWTSQueryUserToken)(ULONG SessionId, PHANDLE phToken);
typedef BOOL (WINAPI *pWTSQuerySessionInformationW)(
  HANDLE hServer, DWORD SessionId,
  WTS_INFO_CLASS WTSInfoClass,
  LPWSTR **ppBuffer,
  DWORD *pBytesReturned);
typedef BOOL (WINAPI *pWTSQuerySessionInformationA)(
  HANDLE hServer, DWORD SessionId,
  WTS_INFO_CLASS WTSInfoClass,
  LPSTR **ppBuffer,
  DWORD *pBytesReturned);
typedef VOID (WINAPI *pWTSFreeMemory)(void *buffer);

#ifdef UNICODE
#define pWTSQuerySessionInformation pWTSQuerySessionInformationW
#else
#define pWTSQuerySessionInformation pWTSQuerySessionInformationA
#endif


/**
 * Wrapper over WTS WinAPI functions.
 *
 * @author enikey.
 */
class WTS
{
public:
  /**
   * Gets active console session id.
   * @return active console session id if WTS is avaliable or 0 if 
   * WinAPI WTSGetActiveConsoleSessionId function not avaliable.
   */
  static DWORD getActiveConsoleSessionId(LogWriter *log);

  /**
   * Queries user token in active console session.
   * @param token [out] output user token parameter.
   * @throws SystemException on fail.
   * @remark if WTSQueryUserToken is avaliable when it will be used with
   * session id equal to active console session id, if not, then user
   * process id will be used to get user token (this id can be set by using of
   * defineConsoleUserProcessId() method).
   */
  static void queryConsoleUserToken(HANDLE *token, LogWriter *log) throw(SystemException);

  /**
   * Defines global (for WTS class) user process that will be used
   * for getting token of user in active console session if WTSQueryUserToken WinAPI function
   * is unavaliable (Windows 2000 case).
   * @param userProcessId user process id.
   */
  static void defineConsoleUserProcessId(DWORD userProcessId);

  // This function dupplicate token impersonated to named pipe cliend end.
  // This should work only for win2000 because other windows version have
  // rdp.
  static void duplicatePipeClientToken(HANDLE pipeHandle);

  static bool getCurrentUserName(StringStorage *userName, LogWriter *log);

private:
  /**
   * Don't allow instanizing of WTS class.
   */
  WTS();

  /**
   * Initializes WTS functions.
   */
  static void initialize(LogWriter *log);

  // The initialize() function should be already called before use the wtsFreeMemory() function.
  static void wtsFreeMemory(void *buffer);

  static DynamicLibrary *m_kernel32Library;
  static DynamicLibrary *m_wtsapi32Library;
  static pWTSGetActiveConsoleSessionId m_WTSGetActiveConsoleSessionId;
  static pWTSQueryUserToken m_WTSQueryUserToken;
  static pWTSQuerySessionInformation m_WTSQuerySessionInformation;
  static pWTSFreeMemory m_WTSFreeMemory;

  /**
   * Determinates if WTS library was initialized.
   */
  static volatile bool m_initialized;

  /**
   * Token of interactive user process in active console session
   * that will be used if WTSQueryToken WinAPI function is unavaliable
   * in queryConsoleUserToken() call.
   */
  static HANDLE m_userProcessToken;

  /**
   * Thread-safety.
   */
  static LocalMutex m_mutex;
};

#endif
