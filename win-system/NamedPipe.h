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

#ifndef __NAMEDPIPE_H__
#define __NAMEDPIPE_H__

#include "util/CommonHeader.h"
#include "WindowsEvent.h"
#include "io-lib/Channel.h"
#include "thread/LocalMutex.h"
#include "Pipe.h"

/**
 * NamedPipe transport.
 *
 * @author yuri, enikey.
 */
class NamedPipe : public Channel, public Pipe
{
public:
  /**
   * Creates pipe transport.
   */
  NamedPipe(HANDLE hPipe, unsigned int maxPortionSize, bool asServer);
  /**
   * Destroys instance.
   */
  virtual ~NamedPipe();

  /**
   * Closes transport.
   *
   * @throws Exception on fail.
   */
  void close();

  /**
   * Reads data from pipe.
   * Implemented from Channel interface.
   * @param buffer buffer to receive data.
   * @param len count of bytes to read.
   * @throws IOException on io error.
   */
  virtual size_t read(void *buffer, size_t len) throw(IOException);

  /**
   * Writes data to pipe.
   * Implemented from Channel interface.
   * @param buffer buffer with data to write.
   * @param len count of bytes to write.
   * @throws IOException on io error.
   */
  virtual size_t write(const void *buffer, size_t len) throw(IOException);

  virtual size_t available() { return 0; };

  virtual HANDLE getHandle() const;

private:
  void checkPipeHandle();

  HANDLE m_hPipe;
  LocalMutex m_hPipeMutex;
  StringStorage m_pipeName;

  WindowsEvent m_readEvent;
  WindowsEvent m_writeEvent;
  bool m_asServer;
};

#endif // __NAMEDPIPE_H__
