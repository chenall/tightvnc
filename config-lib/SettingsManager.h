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

#ifndef _SETTINGS_MANAGER_
#define _SETTINGS_MANAGER_

#include "util/CommonHeader.h"
#include "util/StringStorage.h"

class SettingsManager
{
public:
  SettingsManager();
  virtual ~SettingsManager();
public:  
  virtual bool isOk() = 0;

  virtual bool keyExist(const TCHAR *name) = 0;
  virtual bool deleteKey(const TCHAR *name) = 0;

  virtual bool getString(const TCHAR *name, StringStorage *storage) = 0;
  virtual bool setString(const TCHAR *name, const TCHAR *value) = 0;

  virtual bool getLong(const TCHAR *name, long *value) = 0;
  virtual bool setLong(const TCHAR *name, long value) = 0;

  virtual bool getBoolean(const TCHAR *name, bool *value) = 0;
  virtual bool setBoolean(const TCHAR *name, bool value) = 0;

  virtual bool getUINT(const TCHAR *name, UINT *value) = 0;
  virtual bool setUINT(const TCHAR *name, UINT value) = 0;

  virtual bool getInt(const TCHAR *name, int *value) = 0;
  virtual bool setInt(const TCHAR *name, int value) = 0;

  virtual bool getByte(const TCHAR *name, char *value) = 0;
  virtual bool setByte(const TCHAR *name, char value) = 0;

  virtual bool getBinaryData(const TCHAR *name, void *value, size_t *size) = 0;
  virtual bool setBinaryData(const TCHAR *name, const void *value, size_t size) = 0;
};

#endif
