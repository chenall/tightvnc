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

#include "RfbPointerEventClientMessage.h"

RfbPointerEventClientMessage::RfbPointerEventClientMessage(const UINT8 buttonMask,
                                                           const Point *position)
: m_buttonMask(buttonMask)
{
  m_xPos = static_cast<UINT16>(position->x);
  m_yPos = static_cast<UINT16>(position->y);
}

RfbPointerEventClientMessage::~RfbPointerEventClientMessage()
{
}

void RfbPointerEventClientMessage::send(RfbOutputGate *output)
{
  AutoLock al(output);
  output->writeUInt8(ClientMsgDefs::POINTER_EVENT);
  output->writeUInt8(m_buttonMask);
  output->writeUInt16(m_xPos);
  output->writeUInt16(m_yPos);
  output->flush();
}
