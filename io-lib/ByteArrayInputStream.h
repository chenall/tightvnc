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

#ifndef _BYTE_ARRAY_INPUT_STREAM_H_
#define _BYTE_ARRAY_INPUT_STREAM_H_

#include "InputStream.h"
#include "IOException.h"

/**
 * Input stream for reading data from memory.
 */
class ByteArrayInputStream : public InputStream
{
public:
  /**
   * Creates new input stream for reading data from memory.
   * @param buffer source buffer.
   * @param bufferSize count of bytes in memory buffer.
   */
  ByteArrayInputStream(const char *buffer, size_t bufferSize);
  virtual ~ByteArrayInputStream();

  /**
   * Reads data from memory.
   * @param buffer target buffer to write data.
   * @param len count of bytes to write.
   * @throws IOException when no data left in memory buffer.
   */
  virtual size_t read(void *buffer, size_t len) throw(IOException);

  virtual size_t available();

protected:
  const char *m_buffer;
  size_t m_bufferSize;
  size_t m_left;
};

#endif
