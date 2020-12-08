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

#ifndef _INI_FILE_SETTINGS_MANAGER_H_
#define _INI_FILE_SETTINGS_MANAGER_H_

#include "SettingsManager.h"

#include "util/StringStorage.h"

class IniFileSettingsManager : public SettingsManager
{
public:
  IniFileSettingsManager(const TCHAR *pathToFile, const TCHAR *appName);
  IniFileSettingsManager(const TCHAR *pathToFile);
  IniFileSettingsManager();

  virtual ~IniFileSettingsManager();

  // Sets application name (section name)
  void setApplicationName(const TCHAR *appName);

  // Sets path to ini file
  void setPathToFile(const TCHAR *pathToFile);

  //
  // Inherited from SettingsManager class
  //

  // Return false if path to ini file name not specified
  virtual bool isOk();

  virtual bool keyExist(const TCHAR *name);

  virtual bool deleteKey(const TCHAR *name);

  virtual bool getString(const TCHAR *name, StringStorage *storage);
  // Remark: returns value if value is NULL.
  virtual bool setString(const TCHAR *name, const TCHAR *value);

  virtual bool getLong(const TCHAR *name, long *value);
  virtual bool setLong(const TCHAR *name, long value);

  virtual bool getBoolean(const TCHAR *name, bool *value);
  virtual bool setBoolean(const TCHAR *name, bool value);

  virtual bool getUINT(const TCHAR *name, UINT *value);
  virtual bool setUINT(const TCHAR *name, UINT value);

  virtual bool getInt(const TCHAR *name, int *value);
  virtual bool setInt(const TCHAR *name, int value);

  virtual bool getByte(const TCHAR *name, char *value);
  virtual bool setByte(const TCHAR *name, char value);

  virtual bool getBinaryData(const TCHAR *name, void *value, size_t *size);
  virtual bool setBinaryData(const TCHAR *name, const void *value, size_t size);

protected:
  StringStorage m_appName;
  StringStorage m_pathToFile;

private:

  // Helper method.
  // Sets string returned by GetPrivateProfileString to value out argument.
  //
  // Parameters:
  // [in]  name - key name
  // [out] value - variable where output string will be located
  // [in]  defaultValue -  if key does not exists defaultValue
  //       will be storaged to value argument as output value.
  void getPrivateProfileString(const TCHAR *name, StringStorage *value,
                               const TCHAR *defaultValue);

  //
  // Helper template method to avoid code duplicate
  // in such methods as getByte, getUINT.
  //

  template<typename T> bool getIntAndCastTo(const TCHAR *name, T *value) {
    int intValue = 0;
    if (!getInt(name, &intValue)) {
      return false;
    }
    *value = (T)intValue;
    return true;
  };

};

#endif
