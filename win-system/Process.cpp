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

#include "Process.h"
#include "SystemException.h"

Process::Process(const TCHAR *path, const TCHAR *args)
: m_hProcess(0),
  m_hThread(0),
  m_handlesIsInherited(false),
  m_hStopWait(0),
  m_stdIn(0),
  m_stdOut(0),
  m_stdErr(0)
{
  setFilename(path);
  setArguments(args);

  m_hStopWait = CreateEvent(0, FALSE, FALSE, 0);
}

Process::~Process()
{
  cleanup();

  if (m_hStopWait) {
    CloseHandle(m_hStopWait);
  }
}

void Process::setFilename(const TCHAR *path)
{
  m_path.setString(path);
}

void Process::setArguments(const TCHAR *args)
{
  m_args.setString(args);
}

void Process::setStandardIoHandles(HANDLE stdIn, HANDLE stdOut, HANDLE stdErr)
{
  m_stdIn = stdIn;
  m_stdOut = stdOut;
  m_stdErr = stdErr;
}

void Process::setHandleInheritances(bool handlesIsInerited)
{
  m_handlesIsInherited = handlesIsInerited;
}

void Process::getStartupInfo(STARTUPINFO *sti)
{
  ZeroMemory(sti, sizeof(STARTUPINFO));
  sti->cb = sizeof(STARTUPINFO);
  sti->hStdError = m_stdErr;
  sti->hStdInput = m_stdIn;
  sti->hStdOutput = m_stdOut;
  if (sti->hStdError != 0 || sti->hStdInput != 0 || sti->hStdOutput != 0) {
    sti->dwFlags |= STARTF_USESTDHANDLES;
  }
}

void Process::start()
{
  cleanup();

  STARTUPINFO sti;
  getStartupInfo(&sti);
  PROCESS_INFORMATION pi;

  StringStorage commandLine = getCommandLineString();

  _ASSERT(!commandLine.isEmpty());
  if (CreateProcess(NULL, (LPTSTR) commandLine.getString(),
                    NULL, NULL, m_handlesIsInherited, NULL, NULL, NULL,
                    &sti, &pi) == 0) {
    throw SystemException();
  }

  m_hThread = pi.hThread;
  m_hProcess = pi.hProcess;
}

void Process::kill()
{
  if (TerminateProcess(m_hProcess, 0) == 0) {
    throw SystemException();
  }
}

void Process::waitForExit()
{
  HANDLE handleArray[2] = { m_hProcess, m_hStopWait };

  WaitForMultipleObjects(2, handleArray, FALSE, INFINITE);
}

void Process::stopWait()
{
  SetEvent(m_hStopWait);
}

DWORD Process::getExitCode()
{
  DWORD exitCode;

  if (GetExitCodeProcess(m_hProcess, &exitCode) == 0) {
    throw SystemException();
  }

  return exitCode;
}

HANDLE Process::getProcessHandle()
{
  return m_hProcess;
}

StringStorage Process::getCommandLineString()
{
  StringStorage result;

  if (m_args.isEmpty()) {
    return m_path;
  }

  result.format(_T("%s %s"), m_path.getString(), m_args.getString());

  return result;
}

void Process::cleanup()
{
  if (m_hProcess) {
    CloseHandle(m_hProcess);
    m_hProcess = 0;
  }
  if (m_hThread) {
    CloseHandle(m_hThread);
    m_hThread = 0;
  }
}
