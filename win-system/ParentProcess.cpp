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

#include "ParentProcess.h"

ParentProcess::ParentProcess()
: m_isInitialized(false)
{
  init();
}

ParentProcess::~ParentProcess()
{
}

void ParentProcess::init()
{
  HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
  if (hSnapshot != INVALID_HANDLE_VALUE) {
    PROCESSENTRY32 selfPe;
    selfPe.dwSize = sizeof(PROCESSENTRY32);
    DWORD selfId = GetCurrentProcessId();
    // Find the self process entry to get parent process id.
    if (getProcessEntry(&selfPe, selfId, hSnapshot)) {
      // Find the parent process entry
      DWORD parentId = selfPe.th32ParentProcessID;
      m_isInitialized = getProcessEntry(&m_parentPe, parentId, hSnapshot);
    }
    CloseHandle(hSnapshot);
  }
}

bool ParentProcess::getProcessEntry(PROCESSENTRY32 *peOut, DWORD Pid,
                                    HANDLE hSnapshot)
{
  peOut->dwSize = sizeof(PROCESSENTRY32);

  bool result = false;
  if (Process32First(hSnapshot, peOut) == 0) {
    return false;
  }
  do {
    if (peOut->th32ProcessID == Pid) {
      result = true;
    }
  } while (!result && Process32Next(hSnapshot, peOut) != 0);
  return result;
}

bool ParentProcess::isAlive()
{
  bool result = false;
  if (m_isInitialized) {
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot != INVALID_HANDLE_VALUE) {
      PROCESSENTRY32 currParentPe;
      DWORD parentId = m_parentPe.th32ProcessID;
      result = getProcessEntry(&currParentPe, parentId, hSnapshot);
      CloseHandle(hSnapshot);
    }
  }
  return result;
}
