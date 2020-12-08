// Copyright (C) 2010,2011,2012 GlavSoft LLC.
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

#ifndef __AUTHTRACKER_H__
#define __AUTHTRACKER_H__

#include "thread/LocalMutex.h"
#include "util/DateTime.h"

// This class will be insert an time interval between trying of
// authentications
class AuthTracker
{
public:
  AuthTracker(UINT64 failureTimeInterval, unsigned int failureMaxCount);
  virtual ~AuthTracker();

protected:
  // Return ban time if new connection banned and zero if it's not banned.
  UINT64 checkBan();
  // If authentication failed this function must be called to increase an
  // internal failure count.
  void notifyAbAuthFailed();

private:
  // If ban time is elapsed the refresh() function reset m_failureCount.
  void refresh();

  UINT64 m_failureTimeInterval;

  DateTime m_firstFailureTime;
  unsigned int m_failureCount;
  unsigned int m_failureMaxCount;
  LocalMutex m_countMutex;
};

#endif // __AUTHTRACKER_H__
