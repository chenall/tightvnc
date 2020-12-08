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

#ifndef __TVNSERVERAPP_EXTRA_RFB_SERVERS_H__
#define __TVNSERVERAPP_EXTRA_RFB_SERVERS_H__

#include <list>
#include "RfbServer.h"
#include "server-config-lib/PortMappingContainer.h"

class ExtraRfbServers
{
  struct Conf
  {
    bool acceptConnections;
    bool loopbackOnly;
    PortMappingContainer extraPorts;

    Conf();
    Conf(const Conf &other);
    Conf &operator=(const Conf &other);
    bool equals(const Conf *other);
  };

public:
  ExtraRfbServers(LogWriter *log);
  virtual ~ExtraRfbServers();

  // Check current configuration and restart the servers if necessary.
  // Returns true on success (either no work was required or everything has
  // been restarted successfully), false if there were failures on restarting
  // servers (see startUp() for more details).
  bool reload(bool asService, RfbClientManager *mgr);

  // Stop all extra RFB servers, clear the list.
  void shutDown();

protected:
  // Construct and start RFB servers as specified in the Extra Ports
  // configuration. If some servers fail to start, this function does not add
  // it to the internally maintained list of RFB servers.
  // Returns true if all the servers have been started sucessfully, false if
  // at least one failed.
  bool startUp(bool asService, RfbClientManager *mgr);

  // Read configuration into the specified structure.
  static void getConfiguration(Conf *out);

protected:
  std::list<RfbServer *> m_servers;
  Conf m_effectiveConf;

private:
  // Do not allow copying objects.
  ExtraRfbServers(const ExtraRfbServers &);
  ExtraRfbServers &operator=(const ExtraRfbServers &);

  LogWriter *m_log;
};

#endif // __TVNSERVERAPP_EXTRA_RFB_SERVERS_H__
