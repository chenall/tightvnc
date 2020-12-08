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

#include "VncAuthentication.h"

#include "util/AnsiStringStorage.h"
#include "util/DesCrypt.h"

#include <algorithm>

void VncAuthentication::vncAuthenticate(DataInputStream *input,
                                        DataOutputStream *output,
                                        const StringStorage *password)
{
  // TODO: removed duplicate code: ControlAuth.cpp

  // Prepare data for authentication.
  StringStorage truncatedPass;
  password->getSubstring(&truncatedPass, 0, VNC_PASSWORD_SIZE - 1);

  AnsiStringStorage passwordAnsi(&truncatedPass);

  UINT8 m_password[VNC_PASSWORD_SIZE];
  memset(m_password, 0, sizeof(m_password));
  memcpy(m_password, passwordAnsi.getString(),
         std::min(passwordAnsi.getLength(), sizeof(m_password)));

  UINT8 challenge[16];
  UINT8 response[16];

  input->readFully(challenge, sizeof(challenge));
  DesCrypt desCrypt;
  desCrypt.encrypt(response, challenge, sizeof(challenge), m_password);
  output->writeFully(response, sizeof(response));
  output->flush();
}
