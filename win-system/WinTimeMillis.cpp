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

#include "WinTimeMillis.h"

WinTimeMillis::WinTimeMillis(void)
{
  memset(&m_time, 0, sizeof(m_time));
}

WinTimeMillis::~WinTimeMillis(void)
{
}

int WinTimeMillis::diffFrom(const WinTimeMillis *older) const
{
  int olderTime, newTime;
  const SYSTEMTIME *oldTime;

  oldTime = older->getTime();
  olderTime = oldTime->wHour * 3600000 + oldTime->wMinute * 60000 
    + oldTime->wSecond * 1000 + oldTime->wMilliseconds;

  newTime = m_time.wHour * 3600000 + m_time.wMinute * 60000 
    + m_time.wSecond * 1000 + m_time.wMilliseconds;

  return newTime - olderTime;
}
