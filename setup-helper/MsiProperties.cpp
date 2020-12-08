// Copyright (C) 2011,2012 GlavSoft LLC.
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

#include "MsiProperties.h"
#include "util/Exception.h"
#include "util/StringParser.h"
#include <vector>

MsiProperties::MsiProperties(MSIHANDLE handle)
: m_handle(handle)
{
}

MsiProperties::~MsiProperties()
{
}

void MsiProperties::getValue(const TCHAR *name, StringStorage *out)
{
  out->setString(_T(""));
  DWORD charCount = 0;
  UINT retVal = MsiGetProperty(m_handle, name, _T(""), &charCount);
  if (retVal == ERROR_MORE_DATA) {
    charCount++;
    std::vector<TCHAR> value(charCount);
    retVal = MsiGetProperty(m_handle, name, &value.front(), &charCount);
    if (retVal != ERROR_SUCCESS) {
      throw Exception();
    }
    out->setString(&value.front());
  }
}

void MsiProperties::setValue(const TCHAR *name, const StringStorage *value)
{
  UINT retVal = MsiSetProperty(m_handle, name, value->getString());
  if (retVal != ERROR_SUCCESS) {
    throw Exception();
  }
}

void MsiProperties::getString(const TCHAR *name, StringStorage *out)
{
  try {
    getValue(name, out);
  } catch (...) {
    StringStorage errMess;
    errMess.format(_T("Cant't retrieve a string from the %s property"),
                   name);
    throw Exception(errMess.getString());
  }
}

void MsiProperties::setString(const TCHAR *name, const StringStorage *value)
{
  try {
    setValue(name, value);
  } catch (...) {
    StringStorage errMess;
    errMess.format(_T("Cant't set a string to the %s property"),
                   name);
    throw Exception(errMess.getString());
  }
}

int MsiProperties::getInt32(const TCHAR *name)
{
  StringStorage strValue;
  getValue(name, &strValue);
  int retValue = 0;
  if (!StringParser::parseInt(strValue.getString(), &retValue)) {
    StringStorage errMess;
    errMess.format(_T("Can't convert the %s string value to int of the")
                   _T(" %s property"), strValue.getString(), name);
    throw Exception(errMess.getString());
  }
  return retValue;
}

void MsiProperties::setInt32(const TCHAR *name, int value)
{
  try {
    StringStorage valueStr;
    valueStr.format(_T("%d"), value);
    setValue(name, &valueStr);
  } catch (...) {
    StringStorage errMess;
    errMess.format(_T("Cant't set the (int)%d value to the %s property"),
                   value,
                   name);
    throw Exception(errMess.getString());
  }
}
