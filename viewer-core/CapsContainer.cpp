// Copyright (C) 2012 GlavSoft LLC.
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

#include "CapsContainer.h"

#include "thread/AutoLock.h"

CapsContainer::CapsContainer()
{
}

CapsContainer::~CapsContainer()
{
}

void CapsContainer::add(UINT32 code, const char *vendor, const char *name,
                        const StringStorage desc)
{
  // Fill in an RfbCapabilityInfo structure and pass it to the overloaded
  // function.
  RfbCapabilityInfo capinfo;
  capinfo.code = code;
  memcpy(capinfo.vendorSignature, vendor, RfbCapabilityInfo::vendorSigSize);
  memcpy(capinfo.nameSignature, name, RfbCapabilityInfo::nameSigSize);
  add(&capinfo, desc);
}

void CapsContainer::add(const RfbCapabilityInfo *capinfo,
                        const StringStorage desc)
{
  AutoLock al(&m_mapLock);

  infoMap[capinfo->code] = *capinfo;
  enableMap[capinfo->code] = false;
  descMap[capinfo->code] = desc;
}

bool CapsContainer::isAdded(UINT32 code) const
{
  AutoLock al(&m_mapLock);

  return isKnown(code);
}

bool CapsContainer::getInfo(UINT32 code, RfbCapabilityInfo *capinfo)
{
  AutoLock al(&m_mapLock);
  if (isKnown(code)) {
    *capinfo = infoMap[code];
    return true;
  }

  return false;
}

StringStorage CapsContainer::getDescription(UINT32 code) const
{
  AutoLock al(&m_mapLock);
  return (isKnown(code)) ? descMap.find(code)->second : StringStorage();
}

bool CapsContainer::enable(const RfbCapabilityInfo *capinfo)
{
  AutoLock al(&m_mapLock);
  if (!isKnown(capinfo->code)) {
    return false;
  }

  const RfbCapabilityInfo *known = &(infoMap[capinfo->code]);
  if (memcmp(known->vendorSignature, capinfo->vendorSignature,
             RfbCapabilityInfo::vendorSigSize) != 0 ||
      memcmp(known->nameSignature, capinfo->nameSignature,
             RfbCapabilityInfo::nameSigSize) != 0 ) {
    enableMap[capinfo->code] = false;
    return false;
  }

  enableMap[capinfo->code] = true;
  m_plist.push_back(capinfo->code);
  return true;
}

bool CapsContainer::isEnabled(UINT32 code) const
{
  AutoLock al(&m_mapLock);
  return (isKnown(code)) ? enableMap.find(code)->second : false;
}

size_t CapsContainer::numEnabled() const
{
  AutoLock al(&m_mapLock);
  return m_plist.size();
}

UINT32 CapsContainer::getByOrder(size_t idx)
{
  AutoLock al(&m_mapLock);
  return (idx < m_plist.size()) ? m_plist[idx] : 0;
}

void CapsContainer::getEnabledCapabilities(std::vector<UINT32> &codes) const
{
  AutoLock al(&m_mapLock);
  codes = m_plist;
}

bool CapsContainer::isKnown(UINT32 code) const
{
  return (descMap.find(code) != descMap.end());
}
