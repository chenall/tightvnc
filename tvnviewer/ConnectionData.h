// Copyright (C) 2012 GlavSoft LLC.
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

#ifndef _CONNECTION_DATA_
#define _CONNECTION_DATA_

#include "util/StringStorage.h"
#include "rfb/HostPath.h"

class ConnectionData
{
public:
  ConnectionData();
  ConnectionData(const ConnectionData &connectionData);

  //
  // This methods is setter and getter of hostname or pair of hostname and port.
  //
  void setHost(const StringStorage *host);
  StringStorage getHost() const;

  //
  // This method return hostname without port.
  //
  void getReducedHost(StringStorage *strHost) const;

  //
  // This method return port.
  //
  int getPort() const;

  //
  // This method return true, if hostname isn't set.
  //
  bool isEmpty() const;

  //
  // This methods is setter and getters of password.
  //
  StringStorage getPlainPassword() const;
  StringStorage getCryptedPassword() const;
  void setPlainPassword(const StringStorage *password);
  void setCryptedPassword(const StringStorage *password);
  bool isSetPassword() const;
  void resetPassword();

  UINT32 getDispatchId() const;
  bool isSetDispatchId() const;
  void setDispatchId(UINT32 id);
  void unsetDispatchId();

  void setIncoming(bool isIncoming);
  bool isIncoming() const;

protected:
  HostPath m_hostPath;

  // This flag is true, if host isn't set.
  bool m_isEmpty;

  // This flag is true, if password is set.
  bool m_isSetPassword;

  // This flag is true, if connection is incoming (e.g. listening mode).
  bool m_isIncoming;

  // Saved password is crypted.
  StringStorage m_defaultPassword;

  bool m_isSetDispatchId;
  UINT32 m_dispatchId;
};


#endif
