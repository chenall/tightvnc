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

#ifndef __SERVERCOMMANDLINE_H__
#define __SERVERCOMMANDLINE_H__

#include "util/CommandLine.h"

/**
 * TvnServerApplication command line parser.
 */
class ServerCommandLine : public CommandLine
{
public:
  ServerCommandLine();
  virtual ~ServerCommandLine();

  /**
   * Parses command line.
   * @param commandLine command line to parse.
   * @return true if success, false if command line is invalid.
   */
  bool parse(const CommandLineArgs *cmdArgs);

  /**
   * Checks if help flags are set.
   * @return true if any of help flags are set.
   */
  bool showHelp();
};

#endif // __SERVERCOMMANDLINE_H__
