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

#ifndef __SECURITYPIPESERVER_H__
#define __SECURITYPIPESERVER_H__

#include "io-lib/Channel.h"
#include "TimerListener.h"

// The SecurityPipeServer class generates new security pipe connection by
// a granted temporary channel.
class SecurityPipeServer : private TimerListener
{
public:
  // @param tempPublChan is a channel object connected to a client that
  // requires the security pipe channel.
  // @throw Exception on an error.
  // After successful creating this object take the security channel by
  // the getChannel() function to own.
  SecurityPipeServer(Channel *tempPublChan, unsigned int bufferSize);
  virtual ~SecurityPipeServer();

  // Call this function at once to take the security channel. After calling
  // the function the object becomes to useless and may be destroyed.
  Channel *getChannel();

private:
  virtual void onTimeTimer();

  void generateSecConnection(Channel *tempPublChan);
  void makeSure();

  Channel *m_secChannel;
  unsigned int m_bufferSize;
};

#endif // __SECURITYPIPESERVER_H__
