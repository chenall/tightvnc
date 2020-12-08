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

#include "RfbFramebufferUpdateRequestClientMessage.h"

RfbFramebufferUpdateRequestClientMessage::RfbFramebufferUpdateRequestClientMessage
  (bool incremental, Rect updateRect)
: m_incremental(incremental),
  m_rect(updateRect)
{
}

RfbFramebufferUpdateRequestClientMessage::~RfbFramebufferUpdateRequestClientMessage()
{
}

void RfbFramebufferUpdateRequestClientMessage::send(RfbOutputGate * output)
{
  AutoLock al(output);
  output->writeUInt8(ClientMsgDefs::FB_UPDATE_REQUEST);
  output->writeUInt8(m_incremental);
  output->writeUInt16(static_cast<UINT16>(m_rect.left));
  output->writeUInt16(static_cast<UINT16>(m_rect.top));
  output->writeUInt16(static_cast<UINT16>(m_rect.getWidth()));
  output->writeUInt16(static_cast<UINT16>(m_rect.getHeight()));
  output->flush();
}
