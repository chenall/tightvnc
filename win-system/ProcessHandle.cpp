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

#include "ProcessHandle.h"
#include "win-system/SystemException.h"

ProcessHandle::ProcessHandle()
: m_hProcess(0)
{
}

ProcessHandle::~ProcessHandle()
{
  if (m_hProcess != 0) {
    CloseHandle(m_hProcess);
  }
}

void ProcessHandle::openProcess(DWORD dwDesiredAccess,
                                BOOL bInheritHandle,
                                DWORD dwProcessId)
{
  m_hProcess = OpenProcess(dwDesiredAccess, bInheritHandle, dwProcessId);
  if (m_hProcess == 0) {
    StringStorage errMess;
    errMess.format(_T("Can't open the %d process"), dwProcessId);
    throw SystemException(errMess.getString());
  }
}

HANDLE ProcessHandle::getHandle() const
{
  return m_hProcess;
}

void ProcessHandle::getProcessModulePath(StringStorage *exePath)
{
  // FIXME: Test under Windows7
  TCHAR path[MAX_PATH];
  DWORD result = GetModuleFileNameEx(m_hProcess, 0, path,
                                     sizeof(path) / sizeof(TCHAR));
  if (result == 0) {
    throw SystemException(_T("Can't get process module path"));
  }
  exePath->setString(path);
}
