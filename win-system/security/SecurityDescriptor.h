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

#ifndef _SECURITY_DESCRIPTOR_H_
#define _SECURITY_DESCRIPTOR_H_

#include "util/winhdr.h"
#include "win-system/SystemException.h"
#include <AccCtrl.h>

/**
 * Contains the security information associated with an object.
 */
class SecurityDescriptor {
public:
  /**
   * Creates new security descriptor.
   * @remark created security descriptor have no system access control list (SACL),
   * no discretionary access control list (DACL), no owner, no primary group,
   * and all control flags set to FALSE (NULL). Thus, except for its revision level, it is empty
   */
  SecurityDescriptor();
  virtual ~SecurityDescriptor();

  /**
   * Sets rules list for security descriptor.
   * It creates dalc from specified rules and link created dalc with security
   * descriptor using setUserDacl method.
   * @param count count of rules in rules array.
   * @param rules rules array.
   * @throws SystemException on fail.
   */
  void setRulesAsDacl(size_t count,
                      EXPLICIT_ACCESS *rules) throw(SystemException);

  /**
   * Sets information in a discretionary access control list (DACL).
   * Built-in DACL value in acl param cannot be passed.
   * @param acl access control list.
   * @throws SystemException on fail.
   */
  void setUserDacl(ACL *acl) throw(SystemException);

  /**
  Marks the security descriptor as having no owner.
  */
  void clearOwner();

  /**
   * Determines whether the components of a security descriptor are valid.
   */
  bool isValid();

  /**
   Returns pointer to WinAPI security descriptor.
   */
  SECURITY_DESCRIPTOR *getSD();

private:
  SECURITY_DESCRIPTOR m_sd;
};

#endif
