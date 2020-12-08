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

#include "SecurityIdentifier.h"

#include <Sddl.h>
#include <crtdbg.h>
#include <winnt.h>

SecurityIdentifier::SecurityIdentifier(SID *sid)
{
  m_sid = (SID *)LocalAlloc(LPTR, SECURITY_MAX_SID_SIZE);

  memset(m_sid, 0, SECURITY_MAX_SID_SIZE);

  if (!CopySid(SECURITY_MAX_SID_SIZE, m_sid, sid)) {
    throw SystemException();
  }
}

SecurityIdentifier::SecurityIdentifier(const TCHAR *sidString)
: m_sid(0)
{
  getSidByString(sidString, (PSID *)&m_sid);
}

SecurityIdentifier::~SecurityIdentifier()
{
  LocalFree(m_sid);
}

bool SecurityIdentifier::isValid()
{
  return IsValidSid(m_sid) == TRUE;
}

void SecurityIdentifier::toString(StringStorage *sidString)
{
  TCHAR *localAllocatedSidString;

  if (ConvertSidToStringSid(m_sid, &localAllocatedSidString) == FALSE) {
    throw SystemException();
  }

  sidString->setString(localAllocatedSidString);

  LocalFree(localAllocatedSidString);
}

// FIXME: refactor this method.
SecurityIdentifier *SecurityIdentifier::getProcessOwner(HANDLE processHandle)
{
  HANDLE procToken;

  if (OpenProcessToken(processHandle, TOKEN_QUERY, &procToken)) {
    try {
      char buffer[1024];
      DWORD retLen = 0;
      if (!GetTokenInformation(procToken, TokenUser, &buffer, sizeof(buffer), &retLen)) {
        throw SystemException();
      }
      CloseHandle(procToken);
      return new SecurityIdentifier((SID *)((TOKEN_USER *)buffer)->User.Sid);
    } catch (...) {
      CloseHandle(procToken);
      throw;
    }
    CloseHandle(procToken);
  } else {
    throw SystemException();
  }
}

SecurityIdentifier *SecurityIdentifier::createSidFromString(const TCHAR *sidString)
{
  return new SecurityIdentifier(sidString);
}

SID *SecurityIdentifier::getSid() const
{
  return m_sid;
}

void SecurityIdentifier::getSidByString(const TCHAR *sidString, PSID *sid)
{
  if (ConvertStringSidToSid(sidString, sid) == FALSE) {
    throw SystemException();
  }

  _ASSERT(IsValidSid(*sid));
}
