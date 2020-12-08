// Copyright (C) 2014 GlavSoft LLC.
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

#include "DispatchIdProvider.h"

DispatchIdProvider::DispatchIdProvider()
: m_id(0),
  m_idSet(false)
{
}

DispatchIdProvider::DispatchIdProvider(UINT32 id)
: m_id(id),
  m_idSet(true)
{
}

void DispatchIdProvider::setId(UINT32 id)
{
  m_id = id;
  m_idSet = true;
}

bool DispatchIdProvider::getDispatchData(UINT32 *id, StringStorage *dispatcherName, StringStorage *keyword)
{
  if (m_idSet) {
    *id = m_id;
    dispatcherName->setString(_T(""));
    keyword->setString(_T(""));
  }

  return m_idSet;
}
