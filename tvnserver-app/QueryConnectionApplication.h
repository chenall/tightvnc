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

#ifndef _QUERY_CONNECTION_APPLICATION_H_
#define _QUERY_CONNECTION_APPLICATION_H_

#include "win-system/LocalWindowsApplication.h"

#include "win-system/SystemException.h"

/**
 * Application that shows "Accept / Reject rfb connection dialog" to user
 * and returns user choise as application return value.
 */
class QueryConnectionApplication : public LocalWindowsApplication
{
public:
  /**
   * Creates instance of application.
   * @remark behavour inherited from superclass.
   * @throws SystemException on fail.
   */
  QueryConnectionApplication(HINSTANCE hInstance,
                             const TCHAR *windowClassName,
                             const TCHAR *cmdLine);
  /**
   * Deletes instance.
   */
  virtual ~QueryConnectionApplication();

  /**
   * Inherited from superclass.
   *
   * @return 0 when user chooses "accept", 1 when user chooses "reject".
   */
  virtual int run();

  /**
   * Executes query application in separate process
   * and waits until it stops.
   * @param peerAddr string with incoming connection peer address.
   * @param acceptByDefault if set, then default action in query dialog will be "Accept".
   * @param timeOutSec query dialog timeout(in seconds).
   * @return application exit code (0 means accept connection, 1 means reject connection).
   * @throws Exception on fail.
   */
  static int execute(const TCHAR *peerAddr, bool acceptByDefault, DWORD timeOutSec)
         throw(Exception);

private:
  StringStorage m_cmdLine;
};

#endif
