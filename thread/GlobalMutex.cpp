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

#include "GlobalMutex.h"
#include "win-system/SystemException.h"
#include <Aclapi.h>

GlobalMutex::GlobalMutex(const TCHAR *name, bool interSession, bool throwIfExist)
{
  StringStorage mutexName;

  mutexName.format(_T("%s\\%s"), interSession ? _T("Global") : _T("Local"), name);

  m_mutex = CreateMutex(0, FALSE, mutexName.getString());

  if (m_mutex == 0) {
    throw SystemException();
  }

  if (GetLastError() != ERROR_ALREADY_EXISTS) {
    setAccessToAll(m_mutex);
  } else if (throwIfExist) {
    throw SystemException();
  }
}

GlobalMutex::~GlobalMutex()
{
  CloseHandle(m_mutex);
}

void GlobalMutex::lock()
{
  WaitForSingleObject(m_mutex, INFINITE);
}

void GlobalMutex::unlock()
{
  ReleaseMutex(m_mutex);
}

void GlobalMutex::setAccessToAll(HANDLE objHandle)
{
  DWORD errorCode = SetSecurityInfo(objHandle, SE_KERNEL_OBJECT,
                                    DACL_SECURITY_INFORMATION, // Modify DACL
                                    0,
                                    0,
                                    0, // Pointer to DACL (0 = access to all)
                                    0);
  if (errorCode != ERROR_SUCCESS) {
    throw SystemException(errorCode);
  }
}
