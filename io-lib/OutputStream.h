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

#ifndef _OUTPUT_STREAM_H_
#define _OUTPUT_STREAM_H_

#include "IOException.h"

/**
 * Output stream interface (abstract class).
 */
class OutputStream
{
public:
  virtual ~OutputStream();

  /**
   * Writes data to stream.
   * @param buffer buffer with data to write.
   * @param len count of bytes to write.
   * @return count of written bytes.
   * @throws any kind of exception (depends on implementation).
   */
  virtual size_t write(const void *buffer, size_t len) = 0;

  /**
   * Flushes inner buffer to real output stream.
   *
   * flush method of OutputStream does nothing, it can be override
   * by subclasses which uses bufferization.
   */
  virtual void flush();
};

#endif
