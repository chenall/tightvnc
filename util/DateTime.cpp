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

#include "DateTime.h"

#include <crtdbg.h>

DateTime::DateTime()
: m_timeValue(0)
{
}
DateTime::DateTime(const DateTime &dt)
: m_timeValue(dt.m_timeValue)
{
}

DateTime::DateTime(UINT64 timeValue)
: m_timeValue(timeValue)
{
}

DateTime::DateTime(FILETIME ft)
{
  const UINT64 SECS_BETWEEN_EPOCHS = 11644473600;
  const UINT64 SECS_TO_100NS = 10000000;

   // Time since windows epoch in 100ns intervals.
  UINT64 winTime100ns = ((UINT64)ft.dwHighDateTime << 32) + ft.dwLowDateTime;

   // Time since unix epoch in 100ns intervals.
  UINT64 unixTime100ns = winTime100ns - SECS_BETWEEN_EPOCHS * SECS_TO_100NS;

   // Time singe unix epoch in milliseconds.
  m_timeValue = (unixTime100ns / 10000);
}

UINT64 DateTime::getTime() const
{
  return m_timeValue;
}

DateTime DateTime::operator- (const DateTime &other)
{
  return DateTime(getTime() - other.getTime());
}

DateTime DateTime::operator+ (const DateTime &other)
{
  return DateTime(getTime() + other.getTime());
}

void DateTime::toFileTime(LPFILETIME ft) const
{
  UINT64 ll;
  ll = m_timeValue * (10000000 / 1000) + 116444736000000000;
  ft->dwLowDateTime = (DWORD)ll;
  ft->dwHighDateTime = ll >> 32;
}

void DateTime::toUtcSystemTime(LPSYSTEMTIME st) const
{
  FILETIME ft;

  toFileTime(&ft);

  FileTimeToSystemTime(&ft, st);
}

void DateTime::toLocalSystemTime(LPSYSTEMTIME st) const
{
  FILETIME ft, localFt;

  toFileTime(&ft);
  // Convert file time from UTC format to local.
  FileTimeToLocalFileTime(&ft, &localFt);

  FileTimeToSystemTime(&localFt, st);
}

void DateTime::toString(StringStorage *target) const
{
  SYSTEMTIME systemTime;

  toLocalSystemTime(&systemTime);

  const size_t dateStringMaxLength = 255;

  TCHAR dateString[dateStringMaxLength + 1];

  if (GetDateFormat(LOCALE_USER_DEFAULT,
        DATE_SHORTDATE,
        &systemTime,
        0,
        dateString,
        dateStringMaxLength) == 0) {
    // TODO: Process this error.
  }

  target->setString(dateString);
  target->appendChar(_T(' '));

  const size_t timeStringMaxLength = 255;

  TCHAR timeString[timeStringMaxLength + 1];

  if (GetTimeFormat(
        LOCALE_USER_DEFAULT,
        0,
        &systemTime,
        0,
        timeString,
        timeStringMaxLength) == 0) {
    // TODO: Process this error.
  }

  target->appendString(timeString);
}

DateTime DateTime::now()
{
  SYSTEMTIME st = {0};

  GetLocalTime(&st);

  FILETIME ft;

  SystemTimeToFileTime(&st, &ft);

  DateTime dt(ft);

  return dt;
}
