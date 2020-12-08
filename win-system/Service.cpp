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

#include "Service.h"

#include "win-system/SystemException.h"

#include <crtdbg.h>

#include "SCMClient.h"

Service *Service::g_service = 0;

void WINAPI Service::ServiceMain(DWORD dwArgc, LPTSTR *lpszArgv)
{
  g_service->m_statusHandle = RegisterServiceCtrlHandler(g_service->m_name.getString(),
                                                         &Service::ServiceControlHandler);

  if (!g_service->m_statusHandle) {
    throw SystemException();
  }

  g_service->m_status.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
  g_service->m_status.dwServiceSpecificExitCode = 0;

  try {
    g_service->onStart();
  } catch (Exception &) {
    g_service->reportStatus(SERVICE_STOPPED, NO_ERROR, 0);
    // TODO: Report to SCManager about critical error.
  }

  g_service->reportStatus(SERVICE_RUNNING, NO_ERROR, 0);

  try {
    g_service->main();
  } catch (Exception &) {
    g_service->reportStatus(SERVICE_STOPPED, NO_ERROR, 0);
    // TODO: Report to SCManager about critical error.
  }

  g_service->reportStatus(SERVICE_STOPPED, NO_ERROR, 0);
}

void WINAPI Service::ServiceControlHandler(DWORD dwCtrlCode)
{
  if (dwCtrlCode == SERVICE_CONTROL_STOP) {
    Service::g_service->reportStatus(SERVICE_STOP_PENDING, NO_ERROR, 0);

    Service::g_service->onStop();
  }
}

bool Service::reportStatus(DWORD dwCurrentState, DWORD dwWin32ExitCode, DWORD dwWaitHint)
{
  if (dwCurrentState == SERVICE_START_PENDING) {
    m_status.dwControlsAccepted = 0;
  } else {
    m_status.dwControlsAccepted = SERVICE_ACCEPT_STOP;
  }

  m_status.dwCurrentState = dwCurrentState;
  m_status.dwWin32ExitCode = dwWin32ExitCode;
  m_status.dwWaitHint = dwWaitHint;

  if ((dwCurrentState == SERVICE_RUNNING) || (dwCurrentState == SERVICE_STOPPED)) {
    m_status.dwCheckPoint = 0;
  } else {
    m_status.dwCheckPoint++;
  }

  return SetServiceStatus(m_statusHandle, &m_status) == TRUE;
}

Service::Service(const TCHAR *name)
{
  _ASSERT(Service::g_service == NULL);

  Service::g_service = this;

  m_name.setString(name);
}

Service::~Service()
{
  Service::g_service = 0;
}

void Service::run()
{
  TCHAR name[1024];

  _tcscpy_s(name, 1024, m_name.getString());

  SERVICE_TABLE_ENTRY dispatchTable[] =  {{name, (LPSERVICE_MAIN_FUNCTION)ServiceMain }, { NULL, NULL }};

  if (!StartServiceCtrlDispatcher(dispatchTable)) {
    throw SystemException();
  }
}
