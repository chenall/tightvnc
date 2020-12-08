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

#ifndef _QUERY_CONNECTION_COMMAND_LINE_H_
#define _QUERY_CONNECTION_COMMAND_LINE_H_

#include "util/CommonHeader.h"
#include "util/CommandLine.h"
#include "util/Exception.h"

class QueryConnectionCommandLine : private CommandLine
{
public:
  static const TCHAR QUERY_CONNECTION[];
  static const TCHAR PEER_ADDR[];
  static const TCHAR TIMEOUT[];
  static const TCHAR ACCEPT[];

public:
  QueryConnectionCommandLine();
  virtual ~QueryConnectionCommandLine();

  void parse(const CommandLineArgs *commandLine) throw(Exception);

  // Returns true if timeout is specified.
  bool isTimeoutSpecified();

  // Stores ip address value into storage output parameter.
  void getPeerAddress(StringStorage *storage);
  // Returns true if default action is accept.
  bool isDefaultActionAccept();
  // Returns timeout in seconds.
  DWORD getTimeout();

protected:
  DWORD m_timeout;
};

#endif
