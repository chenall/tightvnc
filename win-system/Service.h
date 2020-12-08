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

#ifndef _SERVICE_H_
#define _SERVICE_H_

#include "util/winhdr.h"
#include "util/StringStorage.h"
#include "util/Exception.h"
#include "thread/Thread.h"

/**
 * Win32 service class (abstract).
 * @usage create service subclass, instanize subclass and call run() method.
 * @remark service is singleton (you can have only one service instance).
 * @author enikey.
 */
class Service
{
public:
  /**
   * Creates new Service class instance.
   * @remark pointer to global service (singleton) saves here.
   * @param name name of service.
   */
  Service(const TCHAR *name);

  /**
   * Deletes service instance.
   * @remark releases singleton pointer.
   */
  virtual ~Service();

  /**
   * Starts service execution.
   */
  void run() throw(Exception);

protected:
  /**
   * Called from service control manager when service needs to start.
   */
  virtual void onStart() = 0;
  /**
   * Service main.
   */
  virtual void main() = 0;
  /**
   * Called from service control manager when service needs to stop/
   */
  virtual void onStop() = 0;

  /**
   * Win32 API service main function.
   * @see MSDN for details.
   */
  static void WINAPI ServiceMain(DWORD dwArgc, LPTSTR *lpszArgv);
  /**
   * Win32 API service control handler.
   * @see MSDN for details.
   */
  static void WINAPI ServiceControlHandler(DWORD dwCtrlCode);

protected:
  /**
   * @fixme add comment to it.
   */
  bool reportStatus(DWORD dwCurrentState, DWORD dwWin32ExitCode,
                    DWORD dwWaitHint);

protected:
  /**
   * Service name for SCM.
   */
  StringStorage m_name;

  /**
   * Current service status.
   */
  SERVICE_STATUS m_status;
  /**
   * Service handle.
   */
  SERVICE_STATUS_HANDLE m_statusHandle;

  /**
   * Flag determinates if service is terminating.
   */
  volatile bool m_isTerminating;

  /**
   * Service (global instance).
   */
  static Service *g_service;
};

#endif
