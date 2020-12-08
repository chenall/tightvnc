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

#include "BufferedOutputStream.h"

BufferedOutputStream::BufferedOutputStream(OutputStream *output)
: m_dataLength(0)
{
  m_output = new DataOutputStream(output);
}

BufferedOutputStream::~BufferedOutputStream()
{
  try {
    flush();
  } catch (...) {
  } // try / catch.

  delete m_output;
}

size_t BufferedOutputStream::write(const void *buffer, size_t len)
{
  if (m_dataLength + len >= sizeof(m_buffer)) {
    flush();

    m_output->writeFully(buffer, len);
  } else {
    memcpy(&m_buffer[m_dataLength], buffer, len);

    m_dataLength += len;
  }

  return len;
}

void BufferedOutputStream::flush()
{
  m_output->writeFully(&m_buffer[0], m_dataLength);

  m_dataLength = 0;
}
