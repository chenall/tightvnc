// Copyright (C) 2010,2011,2012 GlavSoft LLC.
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

#include "SecurityPipeClient.h"
#include "io-lib/DataInputStream.h"
#include "io-lib/DataOutputStream.h"
#include "win-system/NamedPipe.h"

SecurityPipeClient::SecurityPipeClient(Channel *tempPublChan, unsigned int maxPortionSize)
: m_secChannel(0)
{
  try {
    DataInputStream svcInput(tempPublChan);
    DataOutputStream svcOutput(tempPublChan);
    // Send our process id
    svcOutput.writeUInt32(GetCurrentProcessId());
    // Get working channel.
    HANDLE pipeHandle = (HANDLE)svcInput.readUInt64();
    m_secChannel = new NamedPipe(pipeHandle, maxPortionSize, false);
    DataOutputStream output(m_secChannel);
    // Send sure message.
    output.writeUInt8(255);
  } catch (...) {
    if (m_secChannel != 0) delete m_secChannel;
    throw;
  }
}

SecurityPipeClient::~SecurityPipeClient()
{
}

Channel *SecurityPipeClient::getChannel()
{
  Channel *chan = m_secChannel;
  m_secChannel = 0;
  return chan;
}
