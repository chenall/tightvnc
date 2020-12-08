// Copyright (C) 2008,2009,2010,2011,2012 GlavSoft LLC.
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

#include <stdlib.h>
#include "SocketStream.h"

#include "../socket/sockdefs.h"

#include <crtdbg.h>

SocketStream::SocketStream(SocketIPv4 *sock)
: m_socket(sock)
{
  _ASSERT(m_socket != NULL);
}

SocketStream::~SocketStream()
{
}

size_t SocketStream::read(void *buf, size_t wanted)
{
  if ((int)wanted < 0) {
    throw IOException(_T("Wanted size too big."));
  }

  return (size_t)m_socket->recv((char *)buf, (int)wanted);
}

size_t SocketStream::write(const void *buf, size_t size)
{
  if ((int)size < 0) {
    throw IOException(_T("Size of buffer is too big."));
  }

  return (size_t)m_socket->send((char *)buf, (int)size);
}

void SocketStream::close()
{
  try {
    m_socket->shutdown(SD_BOTH);
  } catch (...) {
  }
  m_socket->close();
}

size_t SocketStream::available() {
  return m_socket->available();
}
