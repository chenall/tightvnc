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

#include "CapContainer.h"
#include <crtdbg.h>

CapContainer::CapContainer()
{
}

CapContainer::~CapContainer()
{
}

void CapContainer::addCap(UINT32 code, const char *vendorSignature,
                          const char *nameSignature)
{
  _ASSERT(strlen(vendorSignature) == 4);
  _ASSERT(strlen(nameSignature) == 8);
  Cap cap;
  cap.code = code;
  memcpy(cap.vendorSignature, vendorSignature, 4);
  memcpy(cap.nameSignature, nameSignature, 8);
  m_caps.push_back(cap);
}

unsigned int CapContainer::getCapCount() const
{
  unsigned int capsSize = (unsigned int)m_caps.size();
  _ASSERT(capsSize == m_caps.size());
  return capsSize;
}

void CapContainer::sendCaps(DataOutputStream *output) const
{
  for (CapVectorConstIter iter = m_caps.begin(); iter < m_caps.end(); iter++) {
    output->writeUInt32((*iter).code);
    output->writeFully(&(*iter).vendorSignature, 4);
    output->writeFully(&(*iter).nameSignature, 8);
  }
}

bool CapContainer::includes(UINT32 code) const
{
  bool result = false;
  for (CapVectorConstIter iter = m_caps.begin(); iter < m_caps.end(); iter++) {
    if ((*iter).code == code) {
      result = true;
      break;
    }
  }
  return result;
}
