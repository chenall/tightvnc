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

#ifndef _RFB_OUTPUT_GATE_H_
#define _RFB_OUTPUT_GATE_H_

#include "io-lib/DataOutputStream.h"
#include "io-lib/BufferedOutputStream.h"

#include "thread/LocalMutex.h"

/**
 * Gate for writting rfb messages.
 *
 * @features: gate is synchonized (can be locked and unlocked, supports data buffering, and writting
 * typized data).
 * @remark: after every message you want to send to must manually call flush() cause
 * "autoflush on unlock" is removed.
 * @author enikey.
 */
class RfbOutputGate : public DataOutputStream,
                      public LocalMutex
{
public:
  /**
   * Creates new rfb output gate.
   * @param stream real output stream.
   */
  RfbOutputGate(OutputStream *stream);
  /**
   * Deletes rfb output gate.
   */
  virtual ~RfbOutputGate();

  /**
   * Flushes inner buffer to real output stream.
   * @throws IOException on error.
   */
  virtual void flush() throw(IOException);

private:
  /**
   * Tunnel that adds buffering.
   */
  BufferedOutputStream *m_tunnel;
};

#endif
