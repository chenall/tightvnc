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

#ifndef _DATE_TIME_H_
#define _DATE_TIME_H_

#include "inttypes.h"
#include "winhdr.h"
#include "StringStorage.h"

/**
 * DateTime class.
 */
class DateTime
{
public:
  /**
   * Creates empty DateTime.
   */
  DateTime();

  DateTime(const DateTime &dt);

  /**
   * Creates DateTime object with specified time value.
   * @param timeValue count of milliseconds from unix epoch.
   */
  DateTime(UINT64 timeValue);

  /**
   * Creates DateTime object from windows FILETIME structure.
   * @param ft time as FILETIME structure.
   */
  DateTime(FILETIME ft);

  /**
   * Returns time in milliseconds from unix epoch.
   */
  UINT64 getTime() const;

  DateTime operator- (const DateTime &d2);
  DateTime operator+ (const DateTime &d2);

  /**
   * Converts this DateTime to FILETIME structure.
   * @param [out] ft output parameter.
   */
  void toFileTime(LPFILETIME ft) const;

  /**
   * Converts this DateTime to SYSTEMTIME structure in UTC format.
   * @param [out] st output parameter.
   */
  // FIXME: Rename this method because the now() function already
  // returns in the local time and to convert after that this function
  // must be used but time do not converting to UTC in this case.
  void toUtcSystemTime(LPSYSTEMTIME st) const;

  /**
   * Converts this DateTime to SYSTEMTIME structure in local format.
   * @param [out] st output parameter.
   */
  void toLocalSystemTime(LPSYSTEMTIME st) const;
  /**
   * Converts this DateTime to string.
   * @param [out] out output parameter.
   */
  void toString(StringStorage *out) const;

  /**
   * Returns current local time.
   */
  static DateTime now();

protected:
  UINT64 m_timeValue;
};

#endif
