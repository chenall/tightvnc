// Copyright (C) 2008,2009,2010,2011,2012 GlavSoft LLC.
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

#ifndef _REGISTRY_SETTINGS_MANAGER_
#define _REGISTRY_SETTINGS_MANAGER_

#include "util/StringStorage.h"
#include "win-system/RegistryKey.h"

#include "SettingsManager.h"

class RegistrySettingsManager : public SettingsManager
{
public:
  RegistrySettingsManager();
  RegistrySettingsManager(HKEY rootKey, const TCHAR *entry, SECURITY_ATTRIBUTES *sa = 0);
  virtual ~RegistrySettingsManager();

public:

  void setRegistryKey(HKEY rootKey, const TCHAR *entry, SECURITY_ATTRIBUTES *sa = 0);

  virtual bool isOk();

  virtual bool keyExist(const TCHAR *name);
  virtual bool deleteKey(const TCHAR *name);

  virtual bool getString(const TCHAR *name, StringStorage *value);
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
  RegistryKey *m_key;

  void extractKeyName(const TCHAR *key, StringStorage *folder);
  void extractValueName(const TCHAR *key, StringStorage *keyName);
};

#endif
