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

#ifndef __SECURITYPIPECLIENT_H__
#define __SECURITYPIPECLIENT_H__

#include "io-lib/Channel.h"

// The SecurityPipeClient class gets new security pipe connection by
// a granted temporary channel.
class SecurityPipeClient
{
public:
  // @param tempPublChan is a channel object connected to a server  from that
  // we receive the security pipe channel.
  // After successful creating this object take the security channel by
  // the getChannel() function to own.
  // @throw Exception on an error, the security channel at this case will be
  // destroyed.
  SecurityPipeClient(Channel *tempPublChan, unsigned int maxPortionSize);
  virtual ~SecurityPipeClient();

  // Call this function at once to take the security channel. After calling
  // the function the object becomes to useless and may be destroyed.
  Channel *getChannel();

private:
  Channel *m_secChannel;
};

#endif // __SECURITYPIPECLIENT_H__
