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

#include "NamedPipeTransport.h"

NamedPipeTransport::NamedPipeTransport(NamedPipe *client)
: m_pipe(client), m_pipeServer(0)
{
}

NamedPipeTransport::NamedPipeTransport(PipeServer *server)
: m_pipe(0), m_pipeServer(server)
{
}

NamedPipeTransport::~NamedPipeTransport()
{
  if (m_pipe != 0) {
    delete m_pipe;
  }
  if (m_pipeServer != 0) {
    delete m_pipeServer;
  }
}

Channel *NamedPipeTransport::getIOStream()
{
  _ASSERT(m_pipe != 0);

  return m_pipe;
}

Transport *NamedPipeTransport::accept()
{
  _ASSERT(m_pipeServer != 0);

  return new NamedPipeTransport(m_pipeServer->accept());
}

void NamedPipeTransport::close()
{
  if (m_pipe != 0) {
    m_pipe->close();
  }
  if (m_pipeServer != 0) {
    m_pipeServer->close();
  }
}
