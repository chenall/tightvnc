// Copyright (C) 2013 GlavSoft LLC.
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

#ifndef __VIEWERVNCAUTHHANDLER_H__
#define __VIEWERVNCAUTHHANDLER_H__

#include "viewer-core/VncAuthenticationHandler.h"
#include "ConnectionData.h"

class ViewerVncAuthHandler : public VncAuthenticationHandler
{
public:
  ViewerVncAuthHandler(ConnectionData *connectionData);
  virtual ~ViewerVncAuthHandler();

private:
  virtual void getPassword(StringStorage *passString);

  ConnectionData *m_connectionData;
};

#endif // __VIEWERVNCAUTHHANDLER_H__
