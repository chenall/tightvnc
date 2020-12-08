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

#ifndef _STRING_TABLE_H_
#define _STRING_TABLE_H_

#include "StringStorage.h"

#include <map>

using namespace std;

/**
 * Helper class with only static methods to get strings from resources.
 */
class StringTable
{
public:
  /**
   * Returns string with specified id from resources.
   * @param id identifier of string in resource file.
   * @return requested string or "Requested string from StringTable cannot be received" if not found.
   * @remark string table hashes loaded strings in private container.
   * @remark this method must be used after ResourceLoader is initialized
   * (or assertion check will fail), cause this method used it.
   * @fixme stub.
   */
  static const TCHAR *getString(UINT id);

private:
  StringTable();

  static map<UINT, StringStorage> _cache;
};

#endif
