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

#include "ClientInputHandler.h"
#include "rfb/MsgDefs.h"

ClientInputHandler::ClientInputHandler(RfbCodeRegistrator *codeRegtor,
                                       ClientInputEventListener *extEventListener,
                                       bool viewOnly)
: m_extEventListener(extEventListener),
  m_viewOnly(viewOnly)
{
  // Request codes
  codeRegtor->regCode(ClientMsgDefs::KEYBOARD_EVENT, this);
  codeRegtor->regCode(ClientMsgDefs::POINTER_EVENT, this);
}

ClientInputHandler::~ClientInputHandler()
{
}

void ClientInputHandler::onRequest(UINT32 reqCode, RfbInputGate *input)
{
  switch (reqCode) {
  case ClientMsgDefs::KEYBOARD_EVENT:
    {
      bool down = input->readUInt8() != 0;
      input->readUInt16(); // Pad
      UINT32 keyCode = input->readUInt32();
      if (!m_viewOnly) {
        m_extEventListener->onKeyboardEvent(keyCode, down);
      }
    }
    break;
  case ClientMsgDefs::POINTER_EVENT:
    {
      UINT8 buttonMask = input->readUInt8();
      UINT16 x = input->readUInt16();
      UINT16 y = input->readUInt16();
      if (!m_viewOnly) {
        m_extEventListener->onMouseEvent(x, y, buttonMask);
      }
    }
    break;
  default:
    StringStorage errMess;
    errMess.format(_T("Unknown %d protocol code received"), (int)reqCode);
    throw Exception(errMess.getString());
    break;
  }
}
