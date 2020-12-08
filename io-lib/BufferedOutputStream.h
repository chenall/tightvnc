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

#ifndef _BUFFERED_OUTPUT_STREAM_H_
#define _BUFFERED_OUTPUT_STREAM_H_

#include "OutputStream.h"
#include "DataOutputStream.h"

/**
 * Buffered output stream class (decorator pattern).
 * Adds bufferization feature to output stream.
 * @remark size of buffer now is fixed and equals to 1400 bytes.
 */
class BufferedOutputStream : public OutputStream
{
public:
  /**
   * Creates new buffered output stream.
   * @param output real output stream.
   */
  BufferedOutputStream(OutputStream *output);
  virtual ~BufferedOutputStream();

  /**
   * Writes data to output stream (with buffering).
   * @remark method can write data to inner buffer, not to real output stream.
   * data will be written to real output stream it inner buffer will overflow.
   * @throw IOException on error.
   * @fixme really it can throw any kind of exception.
   */
  virtual size_t write(const void *buffer, size_t len) throw(IOException);

  /**
   * Writes content of inner buffer to real output stream.
   * @throws IOException on error.
   * @fixme really it can throw any kind of exception.
   */
  void flush() throw(IOException);

protected:
  DataOutputStream *m_output;

  char m_buffer[100000];

  size_t m_dataLength;
};

#endif
