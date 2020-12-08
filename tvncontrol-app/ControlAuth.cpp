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

#include "ControlAuth.h"
#include "util/AnsiStringStorage.h"
#include "tvncontrol-app/ControlProto.h"

#include "thread/AutoLock.h"

#include "util/DesCrypt.h"
#include <algorithm>

ControlAuth::ControlAuth(ControlGate *gate, const TCHAR *password)
: m_gate(gate)
{
  // Prepare data for authentication.
  StringStorage truncatedPass(password);
  truncatedPass.getSubstring(&truncatedPass, 0, ServerConfig::VNC_PASSWORD_SIZE - 1);

  AnsiStringStorage passwordAnsi(&truncatedPass);

  memset(m_password, 0, sizeof(m_password));
  memcpy(m_password, passwordAnsi.getString(),
         std::min(passwordAnsi.getLength(), sizeof(m_password)));

  // FIXME: Why it's commented out?
  // AutoLock l(m_gate);

  m_gate->writeUInt32(ControlProto::AUTH_MSG_ID);
  m_gate->writeUInt32(0);

  authRfb();

  UINT8 result = m_gate->readUInt32();

  switch (result) {
  case ControlProto::REPLY_ERROR:
    {
      StringStorage authFailReason;

      m_gate->readUTF8(&authFailReason);

      throw ControlAuthException(authFailReason.getString());
    }
    break;
  case ControlProto::REPLY_OK:
    break;
  default:
    throw Exception(_T("Server return unknown auth result"));
    break;
  }
}

ControlAuth::~ControlAuth()
{
}

void ControlAuth::authRfb()
{
  UINT8 challenge[16];
  UINT8 response[16];

  m_gate->readFully(challenge, sizeof(challenge));

  DesCrypt desCrypt;

  desCrypt.encrypt(response, challenge, sizeof(challenge), m_password);

  m_gate->writeFully(response, sizeof(response));
}
