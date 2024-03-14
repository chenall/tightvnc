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

#include "CurrentConsoleProcess.h"

#include "win-system/WinStaLibrary.h"
#include "win-system/Environment.h"
#include "win-system/SystemException.h"
#include "win-system/Workstation.h"
#include "win-system/WTS.h"

CurrentConsoleProcess::CurrentConsoleProcess(LogWriter *log, bool connectRdpSession, const TCHAR *path, const TCHAR *args)
: Process(path, args),
  m_log(log),
  m_connectRdpSession(connectRdpSession)
{
}

CurrentConsoleProcess::~CurrentConsoleProcess()
{
}

void CurrentConsoleProcess::start()
{
  cleanup();

  m_log->info(_T("Try to start \"%s %s\" process"),
    m_path.getString(),
    m_args.getString());

  DWORD uiAccess  = 1; // Nonzero enables UI control
  PROCESS_INFORMATION pi;
  STARTUPINFO sti;
  getStartupInfo(&sti);

  m_log->debug(_T("sti: cb = %d, hStdError = %p, hStdInput = %p,")
             _T(" hStdOutput = %p, dwFlags = %u"),
             (unsigned int)sti.cb,
             (void *)sti.hStdError,
             (void *)sti.hStdInput,
             (void *)sti.hStdOutput,
             (unsigned int)sti.dwFlags);

  try {
    HANDLE userToken = WTS::duplicateCurrentProcessUserToken(m_connectRdpSession, m_log);

    StringStorage commandLine = getCommandLineString();

    m_log->debug(_T("Try CreateProcessAsUser(%p, 0, %s, 0, 0, %d, NORMAL_PRIORITY_CLASS, 0, 0,")
               _T(" sti, pi)"),
               (void *)userToken, commandLine.getString(),
               (int)m_handlesIsInherited);
    if (CreateProcessAsUser(userToken, 0, (LPTSTR) commandLine.getString(),
      0, 0, m_handlesIsInherited, NORMAL_PRIORITY_CLASS, 0, 0, &sti,
      &pi) == 0) {
        throw SystemException();
    }
    m_log->info(_T("Created \"%s\" process"), commandLine.getString());
    //
    // FIXME: Leak.
    //
    CloseHandle(userToken);
  } catch (SystemException &sysEx) {
    m_log->error(_T("Failed to start process with %d error"), sysEx.getErrorCode());
    throw;
  }

  m_hThread = pi.hThread;
  m_hProcess = pi.hProcess;
}
