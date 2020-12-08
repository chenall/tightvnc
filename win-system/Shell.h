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

#ifndef _SHELL_H_
#define _SHELL_H_

#include "util/winhdr.h"

#include "SystemException.h"

/**
 * Wrapper over WinAPI shell functions.
 *
 * @author enikey.
 */
class Shell
{
public:
  /**
   * Runs application as administrator and waits until execution finished.
   * @param pathToFile path to executable.
   * @param parameters execute parameters.
   * @throws SystemException on fail.
   */
  static void runAsAdmin(const TCHAR *pathToFile, const TCHAR *parameters) throw(SystemException);

  /**
   * Opens file with default action specified in Windows.
   * @param file target file.
   * @param parameters parameters.
   * @param workDirectory working directory.
   * @throws SystemException on fail.
   */
  static void open(const TCHAR *file,
                   const TCHAR *parameters,
                   const TCHAR *workDirectory) throw(SystemException);
private:
  /**
   * Don't allow instanizing of class.
   */
  Shell();
};

#endif
