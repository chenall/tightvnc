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

#include "ByteArrayInputStream.h"

#include <algorithm>

ByteArrayInputStream::ByteArrayInputStream(const char *buffer, size_t bufferSize)
: m_buffer(buffer), m_bufferSize(bufferSize), m_left(bufferSize)
{
}

ByteArrayInputStream::~ByteArrayInputStream()
{
}

size_t ByteArrayInputStream::read(void *buffer, size_t len)
{
  if (m_left == 0) {
    throw IOException(_T("End of stream reached"));
  }

  if (len < 0) {
    throw IOException(_T("Negative data length value"));
  }

  char *out = (char *)buffer;
  char *in = (char *)m_buffer;

  size_t bytesToCopy = std::min(m_left, len);
  size_t outOffset = 0;
  size_t inOffset = m_bufferSize - m_left;

  memcpy(out + outOffset, in + inOffset, bytesToCopy);

  m_left -= bytesToCopy;

  return bytesToCopy;
}

size_t ByteArrayInputStream::available() {
	return m_left;
}
