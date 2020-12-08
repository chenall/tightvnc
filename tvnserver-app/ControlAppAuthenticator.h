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

#ifndef __CONTROLAPPAUTHENTICATOR_H__
#define __CONTROLAPPAUTHENTICATOR_H__

#include "AuthTracker.h"
#include "win-system/WindowsEvent.h"

// This class will be authenticate insert an time interval between
// failure trying of authentications.
class ControlAppAuthenticator : private AuthTracker
{
public:
  // failureMaxCount is a max count of failure try during a failureTimeInterval
  // without ban. If failure count greater than failureMaxCount then
  // the authenticate() function will be blocked for the failureTimeInterval
  // time elapsed from first failure authentication.
  ControlAppAuthenticator(UINT64 failureTimeInterval,
                          unsigned int failureMaxCount);
  virtual ~ControlAppAuthenticator();

  // Returns true if authentication has succeed and still has not been called
  // the breakAndDisableAuthentications() function.
  // The function may be blocked as described above then if it will
  // be called again the caller will wait other callers in order.
  bool authenticate(const UINT8 cryptPassword[8],
                    const UINT8 challenge[8],
                    const UINT8 response[8]);

  // Breaks all wait operations for this authenticator. Also if some
  // caller calls the authenticate() function it immediately return the
  // false value and will be returns the false value for next calls.
  // Call this function
  // before wait of termination of an object user.
  void breakAndDisableAuthentications();

private:
  // Call this function before process an authentication. If authentication
  // is banned at this time then the funciton will wait until authentication
  // is freed.
  void checkBeforeAuth();

  LocalMutex m_authMutex;
  WindowsEvent m_banDelay;
  bool m_isBreaked;
};

#endif // __CONTROLAPPAUTHENTICATOR_H__
