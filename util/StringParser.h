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

#ifndef _STRING_PARSER_H_
#define _STRING_PARSER_H_

#include "inttypes.h"
#include "CommonHeader.h"

class StringParser
{
public:
  static bool tryParseInt(const TCHAR *str);
  static bool parseInt(const TCHAR *str, int *out);
  // FIXME: it returns true on values that greater then 0xFFFFFFFF.
  static bool parseUInt(const TCHAR *str, unsigned int *out);
  static bool parseUInt64(const TCHAR *str, UINT64 *out);
  static bool parseHex(const TCHAR *str, unsigned int *out);
  static bool parseByte(const TCHAR *str, unsigned char *out);
  static bool parseByteHex(const TCHAR *str, unsigned char *out);
};

#endif
