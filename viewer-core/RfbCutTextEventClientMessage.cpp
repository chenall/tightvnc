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

#include "util/AnsiStringStorage.h"

#include "RfbCutTextEventClientMessage.h"

RfbCutTextEventClientMessage::RfbCutTextEventClientMessage(const StringStorage *cutText)
: m_cutText(*cutText)
{
}

RfbCutTextEventClientMessage::~RfbCutTextEventClientMessage()
{
}

void RfbCutTextEventClientMessage::send(RfbOutputGate *output)
{
  AnsiStringStorage cutTextAnsi;
  cutTextAnsi.fromStringStorage(&m_cutText);
  UINT32 length = static_cast<UINT32>(cutTextAnsi.getLength());

  AutoLock al(output);
  output->writeUInt8(ClientMsgDefs::CLIENT_CUT_TEXT);
  output->writeUInt8(0); // padding 3 bytes
  output->writeUInt8(0);
  output->writeUInt8(0);
  output->writeUInt32(length);
  output->writeFully(cutTextAnsi.getString(), length);
  output->flush();
}
