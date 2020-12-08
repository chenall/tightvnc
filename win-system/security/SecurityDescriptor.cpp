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

#include "SecurityDescriptor.h"

#include <AccCtrl.h>
#include <Aclapi.h>
#include <crtdbg.h>

SecurityDescriptor::SecurityDescriptor()
{
  InitializeSecurityDescriptor(&m_sd, SECURITY_DESCRIPTOR_REVISION);
}

SecurityDescriptor::~SecurityDescriptor()
{
}

void SecurityDescriptor::setRulesAsDacl(size_t count,
                                        EXPLICIT_ACCESS *rules)
{
  PACL acl = 0;

  ULONG constrCount = (ULONG)count;
  _ASSERT(constrCount == count);
  DWORD ret = SetEntriesInAcl(constrCount, rules, NULL, &acl);

  if (ret != ERROR_SUCCESS) {
    throw SystemException(ret);
  }

  setUserDacl(acl);
}

void SecurityDescriptor::setUserDacl(ACL *acl)
{
  if (SetSecurityDescriptorDacl(&m_sd, TRUE, acl,  FALSE) == FALSE) {
    throw SystemException();
  }
}

void SecurityDescriptor::clearOwner()
{
  SetSecurityDescriptorOwner(&m_sd, 0, TRUE);
}

bool SecurityDescriptor::isValid()
{
  return IsValidSecurityDescriptor(&m_sd) == TRUE;
}

SECURITY_DESCRIPTOR *SecurityDescriptor::getSD()
{
  return &m_sd;
}
