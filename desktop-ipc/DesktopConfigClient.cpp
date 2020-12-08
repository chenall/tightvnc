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

#include "DesktopConfigClient.h"
#include "thread/AutoLock.h"
#include "ReconnectException.h"

DesktopConfigClient::DesktopConfigClient(BlockingGate *forwGate)
: DesktopServerProto(forwGate)
{
}

DesktopConfigClient::~DesktopConfigClient()
{
}

void DesktopConfigClient::updateByNewSettings(BlockingGate *gate)
{
  AutoLock al(gate);
  gate->writeUInt8(CONFIG_RELOAD_REQ);
  sendConfigSettings(gate);
}

bool DesktopConfigClient::isRemoteInputAllowed()
{
  bool result = false;
  try {
    AutoLock al(m_forwGate);
    m_forwGate->writeUInt8(SOFT_INPUT_ENABLING_REQ);
    m_forwGate->writeUInt64(m_lastInputTime.getTime());
    result = m_forwGate->readUInt8() != 0;
    m_lastInputTime = DateTime(m_forwGate->readUInt64());
  } catch (ReconnectException &) {
  }
  return result;
}
