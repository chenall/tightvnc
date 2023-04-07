// Copyright (C) 2009,2010,2011,2012,2020 GlavSoft LLC.
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

#include "SetDesktopSize.h"
#include "rfb/MsgDefs.h"
#include "io-lib/DataOutputStream.h"

#include "io-lib/ByteArrayOutputStream.h"
#include "network/RfbOutputGate.h"
#include "network/RfbInputGate.h"
#include "thread/AutoLock.h"
#include "server-config-lib/Configurator.h"
#include "win-system/SystemException.h"
#include "rfb/VendorDefs.h"

SetDesktopSizeRequestHandler::SetDesktopSizeRequestHandler(RfbCodeRegistrator *registrator,
                                                       RfbOutputGate *output,
                                                       LogWriter *log,
                                                       bool enabled)
: m_output(output), m_enabled(enabled),
  m_log(log)
{

  if (!isSetDesktopSizeEnabled()) {
    return ;
  }

  registrator->addClToSrvCap(ClientMsgDefs::SET_DESKTOP_SIZE, VendorDefs::TIGHTVNC, SetDesktopSizeDefs::SET_DESKTOP_SIZE_SIG);

  registrator->regCode(ClientMsgDefs::SET_DESKTOP_SIZE, this);

  m_log->message(_T("SetDesktopSize request handler created"));
}

SetDesktopSizeRequestHandler::~SetDesktopSizeRequestHandler()
{
  m_log->message(_T("SetDesktopSize request handler deleted"));
}

void SetDesktopSizeRequestHandler::onRequest(UINT32 reqCode, RfbInputGate *backGate)
{
  m_input = backGate;

  try {
    if (reqCode == ClientMsgDefs::SET_DESKTOP_SIZE) {
      m_input->readUInt8(); // padding
      UINT16 width = m_input->readUInt16();
      UINT16 height = m_input->readUInt16();
      UINT8 number = m_input->readUInt8(); // number-of-screens
      m_input->readUInt8(); // padding

      for (size_t i = 0; i < number; i++) {
        UINT32 id = m_input->readUInt32();
        UINT16 xpos = m_input->readUInt16();
        UINT16 ypos = m_input->readUInt16();
        UINT16 w = m_input->readUInt16();
        UINT16 h = m_input->readUInt16();
        UINT32 flags = m_input->readUInt32();
      }
      m_log->debug(_T("got SetDesktopSize request"));
      {
        // FIXME: force ExtendedDesktopSize rect send
      }
    }   
  } catch (Exception &someEx) {
    m_log->error(_T("SetDesktopSize extension request failed: \"%s\""), someEx.getMessage());
  } // try / catch.

  m_input = NULL;
}

bool SetDesktopSizeRequestHandler::isSetDesktopSizeEnabled()
{
  return m_enabled;
}


