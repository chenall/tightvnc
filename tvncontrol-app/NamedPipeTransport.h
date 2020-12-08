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

#ifndef _NAMED_PIPE_TRANSPORT_H_
#define _NAMED_PIPE_TRANSPORT_H_

#include "Transport.h"

#include "win-system/Pipe.h"
#include "win-system/PipeServer.h"

/**
 * Transport that uses pipe as transport.
 */
class NamedPipeTransport : public Transport
{
public:
  /**
   * Creates client pipe transport and takes ownership over pipe transport.
   */
  NamedPipeTransport(NamedPipe *client);
  /**
   * Creates server pipe transfer and takes ownership over pipe server.
   */
  NamedPipeTransport(PipeServer *server);
  /**
   * Deletes transport and frees resources.
   */
  virtual ~NamedPipeTransport();

  /**
   * Returns client transport's IO Stream.
   */
  virtual Channel *getIOStream();

  /**
   * Accepts new connection.
   * @return transport for accepted connection.
   * @throws Exception on fail.
   */
  virtual Transport *accept() throw(Exception);

  /**
   * Destroys transport(closes socket).
   */
  virtual void close() throw(Exception);

private:
  /**
   * Connected pipe transport.
   */
  NamedPipe *m_pipe;
  /**
   * Pipe server.
   */
  PipeServer *m_pipeServer;
};

#endif
