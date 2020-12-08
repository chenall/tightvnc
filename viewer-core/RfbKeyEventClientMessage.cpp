// Copyright (C) 2011,2012 GlavSoft LLC.
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

#include "RfbKeyEventClientMessage.h"

RfbKeyEventClientMessage::RfbKeyEventClientMessage(bool downFlag, UINT32 key)
: m_downFlag(downFlag),
  m_key(key)
{
}

RfbKeyEventClientMessage::~RfbKeyEventClientMessage()
{
}

void RfbKeyEventClientMessage::send(RfbOutputGate *output)
{
  AutoLock al(output);
  output->writeUInt8(ClientMsgDefs::KEYBOARD_EVENT);
  output->writeUInt8(m_downFlag);
  output->writeUInt16(0); // padding
  output->writeUInt32(m_key);
  output->flush();
}
