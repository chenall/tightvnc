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

#ifndef _ADDITIONAL_ACTION_APPLICATION_H_
#define _ADDITIONAL_ACTION_APPLICATION_H_

#include "util/CommonHeader.h"

#include "win-system/LocalWindowsApplication.h"

/**
 * Windows application that used to do additional actions:
 *
 * Solves a couple of problems:
 *  1) Lock workstation (-lockworkstation keys).
 *  2) Log out interactive user (-logout keys).
 *
 * @fixme stub class.
 */
class AdditionalActionApplication : public LocalWindowsApplication
{
public:
  /**
   * Key identifying lock workstation action.
   */
  static const TCHAR LOCK_WORKSTATION_KEY[];
  /**
   * Key identifying logout action.
   */
  static const TCHAR LOGOUT_KEY[];

public:
  /**
   * Creates new instance.
   * @param hInstance application instance.
   * @param commandLine command line.
   * @throws SystemException on fail(inherited from superclass).
   */
  AdditionalActionApplication(HINSTANCE hInstance,
                              const TCHAR *windowClassName,
                              const TCHAR *commandLine);
  /**
   * Destructor.
   */
  virtual ~AdditionalActionApplication();

  /**
   * Runs application.
   * @return 0 on success, non-zero on error (Windows error code).
   */
  virtual int run();

private:
  /**
   * Application command line arguments.
   */
  StringStorage m_commandLine;
};

#endif
