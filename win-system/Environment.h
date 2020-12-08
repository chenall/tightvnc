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

#ifndef _ENVIRONMENT_H_
#define _ENVIRONMENT_H_

#include "util/StringStorage.h"
#include "log-writer/LogWriter.h"

#include <winnt.h>

class Environment
{
public:
  static const int APPLICATION_DATA_SPECIAL_FOLDER = 0x0; 
  static const int COMMON_APPLICATION_DATA_SPECIAL_FOLDER = 0x1;
public:
  Environment();
  ~Environment();

  // Formates string from last system error code.
  // @return false on an error.
  static void getErrStr(StringStorage *out);

  // Formates string from last system error code with specification before.
  // @return StringStorage object that contain error describing with
  // secificatioin.
  static void getErrStr(const TCHAR *specification, StringStorage *out);

  //
  // Sets path to special folder value to out argument.
  //
  // Valid values for specialFolderId argument are:
  //  - Environment::APPLICATION_DATA_SPECIAL_FOLDER
  //  - Environment::COMMON_APPLICATION_DATA_SPECIAL_FOLDER
  //
  // Remark: if caller set invalid value into specialFolderId assertion
  // will be failed.
  //

  static bool getSpecialFolderPath(int specialFolderId, StringStorage *out);

  // Sets full path to current executing process file to out argument
  static bool getCurrentModulePath(StringStorage *out);

  // pId - the process id that be tested
  // return true if tested process has the same path as current process
  // else return false
  // throw Exception an error.
  static bool isItTheSamePathAsCurrent(unsigned int pId);

  // Sets full path to folder (without last directory separator character)
  // where current executing process file is located to out argument.
  static bool getCurrentModuleFolderPath(StringStorage *out);

  // Returns user name of the current desktop
  static bool getCurrentUserName(StringStorage *out, LogWriter *log);

  // Fills "out" by computer name string.
  // Returns true if success.
  static bool getComputerName(StringStorage *out);

  // Restores the desktop wallpaper.
  // @throws SystemException on a fail.
  static void restoreWallpaper(LogWriter *log);
  // Hides the desktop wallpaper.
  // @throws SystemException on a fail.
  static void disableWallpaper(LogWriter *log);

  // Returns true if run under a windows NT family
  static bool isWinNTFamily();
  // Returns true if run under windows XP
  static bool isWin2000();
  // Returns true if run under windows XP
  static bool isWinXP();
  // Returns true if run windows 2003 server.
  static bool isWin2003Server();
  // Returns true if run under Windows Vista or later
  static bool isVistaOrLater();

  // Simulates the "ctrl + alt + del" combination under WindowsXP.
  static void simulateCtrlAltDel(LogWriter *log);

  // Simulates the "ctrl + alt + del" combination by using the "SAS" lib.
  static void simulateCtrlAltDelUnderVista(LogWriter *log);

  // Returns true if the Aero is On.
  static bool isAeroOn(LogWriter *log);

private:
  static void init();
  // Operation system version info.
  static OSVERSIONINFO m_osVerInfo;
};

#endif
