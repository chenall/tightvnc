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

#include "Shell.h"

Shell::Shell()
{
}

void Shell::runAsAdmin(const TCHAR *pathToFile, const TCHAR *parameters)
{
  SHELLEXECUTEINFO sei;

  ZeroMemory(&sei, sizeof(sei));

  sei.cbSize = sizeof(SHELLEXECUTEINFOW);
  sei.hwnd = 0;
  sei.fMask = SEE_MASK_FLAG_NO_UI | SEE_MASK_NOCLOSEPROCESS;
  sei.lpVerb = _T("runas");
  sei.lpFile = pathToFile;
  sei.lpParameters = parameters;
  sei.nShow = SW_SHOWNORMAL;

  if (ShellExecuteEx(&sei) == FALSE) {
    throw SystemException();
  }

  WaitForSingleObject(sei.hProcess, INFINITE);

  CloseHandle(sei.hProcess);
}

void Shell::open(const TCHAR *file, const TCHAR *parameters, const TCHAR *workDirectory)
{
  int ret = (int)ShellExecute(0, _T("open"), file, parameters, workDirectory, SW_SHOW);

  if (ret <= 32) {
    throw SystemException(ret);
  }
}
