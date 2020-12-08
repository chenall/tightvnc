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

#include "RegistrySecurityAttributes.h"

#include <crtdbg.h>

RegistrySecurityAttributes::RegistrySecurityAttributes()
{
  EXPLICIT_ACCESS serviceRules[2];
  EXPLICIT_ACCESS applicationRules[2];
  ZeroMemory(&serviceRules, sizeof(serviceRules));
  ZeroMemory(&applicationRules, sizeof(applicationRules));
  fillExplicitAccesses(serviceRules, applicationRules);

  m_serviceSD.setRulesAsDacl(2, serviceRules);
  m_applicationSD.setRulesAsDacl(2, applicationRules);

  m_serviceSA.nLength = sizeof(SECURITY_ATTRIBUTES);
  m_serviceSA.lpSecurityDescriptor = m_serviceSD.getSD();
  m_serviceSA.bInheritHandle = FALSE;

  m_applicationSA.nLength = sizeof(SECURITY_ATTRIBUTES);
  m_applicationSA.lpSecurityDescriptor = m_applicationSD.getSD();
  m_applicationSA.bInheritHandle = FALSE;
}

RegistrySecurityAttributes::~RegistrySecurityAttributes()
{
  delete m_admins;
  delete m_serviceOwner;
  delete m_applicationOwner;
}

void RegistrySecurityAttributes::fillExplicitAccess(EXPLICIT_ACCESS *rules,
                                            const SecurityIdentifier *secIdent)
{
  rules->grfAccessPermissions = KEY_ALL_ACCESS;
  rules->grfAccessMode = SET_ACCESS;
  rules->grfInheritance = SUB_CONTAINERS_AND_OBJECTS_INHERIT;
  rules->Trustee.TrusteeForm = TRUSTEE_IS_SID;
  rules->Trustee.TrusteeType = TRUSTEE_IS_WELL_KNOWN_GROUP;
  rules->Trustee.ptstrName = (LPTSTR)secIdent->getSid();
}

void RegistrySecurityAttributes::fillExplicitAccesses(EXPLICIT_ACCESS serviceRules[2],
                                                      EXPLICIT_ACCESS applicationRules[2])
{
  m_admins = SecurityIdentifier::createSidFromString(_T("S-1-5-32-544"));
  m_serviceOwner = SecurityIdentifier::createSidFromString(_T("S-1-5-18"));
  m_applicationOwner = SecurityIdentifier::getProcessOwner(GetCurrentProcess());

  _ASSERT(m_admins->isValid());
  _ASSERT(m_serviceOwner->isValid());
  _ASSERT(m_applicationOwner->isValid());

  // All access for service.
  fillExplicitAccess(&serviceRules[0], m_admins);
  fillExplicitAccess(&serviceRules[1], m_serviceOwner);
  // All access for application.
  fillExplicitAccess(&applicationRules[0], m_admins);
  fillExplicitAccess(&applicationRules[1], m_applicationOwner);
}

SECURITY_ATTRIBUTES *RegistrySecurityAttributes::getServiceSA()
{
  return &m_serviceSA;
}

SECURITY_ATTRIBUTES *RegistrySecurityAttributes::getApplicationSA()
{
  return &m_applicationSA;
}
