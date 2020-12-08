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

#include "WinHandles.h"
#include "Environment.h"
#include "util/Exception.h"

WinHandles::WinHandles()
{
}

HANDLE WinHandles::assignHandleFor(HANDLE hSource, HANDLE hTargetProc,
                                   bool neededToCloseSource,
                                   bool keepCloseRight)
{
  HANDLE hSrcProc = GetCurrentProcess();
  HANDLE hDest = 0;
  DWORD options = DUPLICATE_SAME_ACCESS;
  if (neededToCloseSource) {
    options |= DUPLICATE_CLOSE_SOURCE;
  }
  if (DuplicateHandle(hSrcProc, hSource, hTargetProc, &hDest, 0, FALSE,
                      options) == 0) {
    StringStorage errText;
    Environment::getErrStr(&errText);
    throw Exception(errText.getString());
  }
  // Try keep of the close rights.
  if (keepCloseRight) {
    if (DuplicateHandle(hTargetProc, hDest, 0, 0, 0, FALSE,
                        DUPLICATE_CLOSE_SOURCE) == 0) {
      StringStorage errText;
      Environment::getErrStr(&errText);
      throw Exception(errText.getString());
    }
  }
  return hDest;
}

HANDLE WinHandles::assignHandleFor(HANDLE hSource,
                                   unsigned int procId,
                                   bool neededToCloseSource,
                                   bool keepCloseRight)
{
  HANDLE processHandle = OpenProcess(PROCESS_DUP_HANDLE, FALSE, procId);
  if (processHandle == 0) {
    throw Exception(_T("Couldn't open process to assign a handle"));
  }
  HANDLE dstHandle;
  try {
    dstHandle = assignHandleFor(hSource, processHandle, neededToCloseSource, keepCloseRight);
    CloseHandle(processHandle);
    return dstHandle;
  } catch (...) {
    CloseHandle(processHandle);
    throw;
  }
}
