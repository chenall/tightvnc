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

#include "ConnectStringParser.h"

#include "util/StringParser.h"

ConnectStringParser::ConnectStringParser()
{
}

ConnectStringParser::~ConnectStringParser()
{
}

bool ConnectStringParser::parse(const TCHAR *connectString,
                                StringStorage *connectHost,
                                unsigned short *connectPort)
{
  StringStorage connStrStorage(connectString);

  size_t len = 2;

  StringStorage splitted[2];

  if (!connStrStorage.split(_T(":"), splitted, &len)) {
    return false;
  }

  if (len != 2) {
    return false;
  }

  int port = 0;

  if (!StringParser::parseInt(splitted[1].getString(), &port)) {
    return false;
  }

  if (port <= 0 || port > 65535) {
    return false;
  }

  if (connectHost != 0) {
    connectHost->setString(splitted[0].getString());
  }

  if (connectPort != 0) {
    *connectPort = (unsigned short)port;
  }

  return true;
}
