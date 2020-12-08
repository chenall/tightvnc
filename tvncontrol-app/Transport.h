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

#ifndef _TRANSPORT_H_
#define _TRANSPORT_H_

#include "io-lib/Channel.h"

#include "util/Exception.h"

/**
 * Abstract control transport that can be used by client and server side
 * both.
 *
 * Hides real transport (Pipe or Socket for example) behind interface.
 *
 * @fixme separate client and server part of transport.
 */
class Transport
{
public:
  /**
   * Transport destructor, does nothing.
   */
  virtual ~Transport();

  /**
   * Returns transport's IO stream for client transport or 0 for
   * server transport.
   */
  virtual Channel *getIOStream() = 0;

  /**
   * Accepts new connection using underlying transport.
   * @return accepted transport or 0 if this transport is client.
   * @throw Exception on fail.
   */
  virtual Transport *accept() = 0;

  /**
   * Closes transport.
   * @throws Exception on fail.
   */
  virtual void close() throw(Exception) = 0;
};

#endif
