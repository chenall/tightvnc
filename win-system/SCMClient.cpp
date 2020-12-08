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

#include "SCMClient.h"

#include "thread/Thread.h"

SCMClientException::SCMClientException(int scmErrCode)
: SystemException(_T("[Exception description is not avaliable]"))
{
  switch (scmErrCode) {
  case ERROR_ALREADY_STOPPED:
  case ERROR_STOP_TIMEOUT:
  case ERROR_ALREADY_RUNNING:
  case ERROR_START_TIMEOUT:
    break;
  default:
    _ASSERT(FALSE);
  };

  m_scmErrCode = scmErrCode;
}

int SCMClientException::getSCMErrorCode() const
{
  return m_scmErrCode;
}

SCMClient::SCMClient(DWORD desiredAccess)
{
  m_managerHandle = OpenSCManager(NULL, NULL, desiredAccess);

  if (m_managerHandle == NULL) {
    throw SystemException();
  }
}

SCMClient::~SCMClient()
{
  if (m_managerHandle != NULL) {
    CloseServiceHandle(m_managerHandle);
  }
}

void SCMClient::installService(const TCHAR *name, const TCHAR *nameToDisplay,
                               const TCHAR *binPath, const TCHAR *dependencies)
{
  SC_HANDLE serviceHandle = CreateService(
    m_managerHandle,              // SCManager database
    name,                         // name of service
    nameToDisplay,               // name to display
    SERVICE_ALL_ACCESS,           // desired access
    SERVICE_WIN32_OWN_PROCESS,
    // service type
    SERVICE_AUTO_START,           // start type
    SERVICE_ERROR_NORMAL,         // error control type
    binPath,                      // service's binary
    NULL,                         // no load ordering group
    NULL,                         // no tag identifier
    dependencies,                 // dependencies
    NULL,                         // LocalSystem account
    NULL);                        // no password

  if (serviceHandle == NULL) {
    throw SystemException();
  }

  // Make the service action to restart on a failure.
  SC_ACTION scAction;
  scAction.Type = SC_ACTION_RESTART; // action on failure
  scAction.Delay = 5000; // Delay before the action

  SERVICE_FAILURE_ACTIONS failureAction;
  failureAction.dwResetPeriod = 0;
  failureAction.lpRebootMsg = 0;
  failureAction.lpCommand = 0;
  failureAction.cActions = 1;
  failureAction.lpsaActions = &scAction;

  ChangeServiceConfig2(serviceHandle, SERVICE_CONFIG_FAILURE_ACTIONS,
                       &failureAction);

  CloseServiceHandle(serviceHandle);
}

void SCMClient::removeService(const TCHAR *name)
{
  try { stopService(name); } catch (...) { }

  SC_HANDLE serviceHandle = OpenService(m_managerHandle, name, SERVICE_ALL_ACCESS);

  if (serviceHandle == NULL) {
    throw SystemException();
  }

  if (!DeleteService(serviceHandle)) {
    CloseServiceHandle(serviceHandle);
    throw SystemException();
  }

  CloseServiceHandle(serviceHandle);

  // Wait until service entry will be removed.

  int triesCount = 0;
  while (true) {
    SC_HANDLE service = OpenService(m_managerHandle, name, SERVICE_ALL_ACCESS);
    if (service == 0) {
      break;
    } else {
      CloseServiceHandle(service);
      Thread::sleep(1000);
      // FIXME: Magic number.
      if (triesCount++ > 10) {
        throw SystemException(1070);
      }
    }
  }
}

void SCMClient::startService(const TCHAR *name, bool waitCompletion)
{
  // FIXME: Wrap SC_HANDLE into a class with a call to CloseServiceHandle()
  //        in the destructor.
  SC_HANDLE serviceHandle = OpenService(m_managerHandle, name, SERVICE_START | SERVICE_QUERY_STATUS);
  if (serviceHandle == NULL) {
    throw SystemException();
  }

  try {
    DWORD state = getServiceState(serviceHandle);
    if (state == SERVICE_RUNNING || state == SERVICE_START_PENDING) {
      throw SCMClientException(SCMClientException::ERROR_ALREADY_RUNNING);
    }
    if (StartService(serviceHandle, 0, 0) == FALSE) {
      throw SystemException();
    }

    if (waitCompletion) {
      int numChecks = 10;
      int msDelayBetweenChecks = 1000;

      while ((state = getServiceState(serviceHandle)) != SERVICE_RUNNING) {
        if (--numChecks <= 0) {
          break;
        }
        Sleep(msDelayBetweenChecks);
      }
      if (state != SERVICE_RUNNING) {
        throw SCMClientException(SCMClientException::ERROR_START_TIMEOUT);
      }
    }
  } catch(...) {
    CloseServiceHandle(serviceHandle);
    throw;
  }

  CloseServiceHandle(serviceHandle);
}

void SCMClient::stopService(const TCHAR *name, bool waitCompletion)
{
  SC_HANDLE serviceHandle = OpenService(m_managerHandle, name, SERVICE_STOP | SERVICE_QUERY_STATUS);
  if (serviceHandle == NULL) {
    throw SystemException();
  }

  try {
    DWORD state = getServiceState(serviceHandle);
    if (state == SERVICE_STOPPED || state == SERVICE_STOP_PENDING) {
      throw SCMClientException(SCMClientException::ERROR_ALREADY_STOPPED);
    }
    SERVICE_STATUS status;
    if (ControlService(serviceHandle, SERVICE_CONTROL_STOP, &status) == FALSE) {
      throw SystemException();
    }

    if (waitCompletion) {
      int numChecks = 10;
      int msDelayBetweenChecks = 1000;

      while ((state = getServiceState(serviceHandle)) != SERVICE_STOPPED) {
        if (--numChecks <= 0) {
          break;
        }
        Sleep(msDelayBetweenChecks);
      }
      if (state != SERVICE_STOPPED) {
        throw SCMClientException(SCMClientException::ERROR_STOP_TIMEOUT);
      }
    }
  } catch(...) {
    CloseServiceHandle(serviceHandle);
    throw;
  }

  CloseServiceHandle(serviceHandle);
}

DWORD SCMClient::getServiceState(SC_HANDLE hService) const
{
  DWORD bytesNeeded;
  SERVICE_STATUS_PROCESS status;

  if (!QueryServiceStatusEx(hService, SC_STATUS_PROCESS_INFO,
                            (LPBYTE)&status, sizeof(status), &bytesNeeded)) {
    throw SystemException();
  }

  return status.dwCurrentState;
}
