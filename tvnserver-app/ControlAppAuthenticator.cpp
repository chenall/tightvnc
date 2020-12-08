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

#include "ControlAppAuthenticator.h"
#include "thread/AutoLock.h"
#include "util/VncPassCrypt.h"

ControlAppAuthenticator::ControlAppAuthenticator(UINT64 failureTimeInterval,
                                                 unsigned int failureMaxCount)
: AuthTracker(failureTimeInterval, failureMaxCount),
  m_isBreaked(false)
{
}

ControlAppAuthenticator::~ControlAppAuthenticator()
{
  breakAndDisableAuthentications();
}

bool ControlAppAuthenticator::authenticate(const UINT8 cryptPassword[8],
                                           const UINT8 challenge[8],
                                           const UINT8 response[8])
{
  AutoLock al(&m_authMutex);

  checkBeforeAuth();
  if (m_isBreaked) {
    return false;
  }

  VncPassCrypt passCrypt;
  passCrypt.updatePlain(cryptPassword);
  bool isAuthSucceed = passCrypt.challengeAndResponseIsValid(challenge,
                                                             response);

  if (!isAuthSucceed) {
    notifyAbAuthFailed();
  }

  return isAuthSucceed;
}

void ControlAppAuthenticator::checkBeforeAuth()
{
  // Wait ban time before authentication
  UINT64 banTime = 1;
  while (banTime != 0 && !m_isBreaked) {
    banTime = checkBan();
    if (banTime != 0) {
      m_banDelay.waitForEvent((DWORD)banTime);
    }
  }
}

void ControlAppAuthenticator::breakAndDisableAuthentications()
{
  m_isBreaked = true;
  m_banDelay.notify();
}
