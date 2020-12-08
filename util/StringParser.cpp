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

#include "StringParser.h"

#include <stdio.h>

bool StringParser::parseInt(const TCHAR *str, int *out)
{
  int value = 0;
  TCHAR c;
  if(_stscanf(str, _T("%d%c"), &value, &c) != 1) {
    return false;
  }
  if (out != NULL) {
    *out = value;
  }

  return true;
}

bool StringParser::parseUInt(const TCHAR *str, unsigned int *out)
{
  // Check the minus sign manually because _tcstoul does not fail on it.
  if (str != 0 && str[0] == _T('-')) {
    return false;
  }

  TCHAR *ptr = 0;
  errno = 0;
  unsigned long ulongValue = _tcstoul(str, &ptr, 10);
  if (errno != 0 || ptr == 0 || *ptr != _T('\0')) {
    return false;
  }

  unsigned int uintValue = (unsigned int)ulongValue;
  if ((unsigned long)uintValue != ulongValue) {
    return false;
  }

  if (out != NULL) {
    *out = uintValue;
  }

  return true;
}

bool StringParser::parseUInt64(const TCHAR *str, UINT64 *out)
{
  UINT64 value = 0;
  TCHAR c;
  if(_stscanf(str, _T("%llu%c"), &value, &c) != 1) {
    return false;
  }
  if (out != NULL) {
    *out = value;
  }

  return true;
}

bool StringParser::tryParseInt(const TCHAR *str)
{
  return parseInt(str, NULL);
}

bool StringParser::parseHex(const TCHAR *str, unsigned int *out)
{
  TCHAR c;
  unsigned int val;
  if (_stscanf(str, _T("%x%c"), &val, &c) != 1) {
    return false;
  }
  if (out != NULL) {
    *out = val;
  }
  return true;
}

bool StringParser::parseByteHex(const TCHAR *str, unsigned char *out)
{
  TCHAR c;
  int val = 0;
  if (_stscanf(str, _T("%x%c"), &val, &c) != 1) {
    return false;
  }
  if (out != NULL) {
    *out = (unsigned char)val;
  }
  return true;
}

bool StringParser::parseByte(const TCHAR *str, unsigned char *out)
{
  TCHAR c;
  int val = 0;
  if (_stscanf(str, _T("%d%c"), &val, &c) != 1) {
    return false;
  }
  if (out != NULL) {
    *out = (unsigned char)val;
  }
  return true;
}
