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

#ifndef _DESKTOP_SERVER_COMMAND_LINE_H_
#define _DESKTOP_SERVER_COMMAND_LINE_H_

#include "util/CommandLine.h"
#include "util/Exception.h"

/**
 * Desktop server command line parser.
 * @valid command line is '-desktopserver -logdir pathToLogDir' where
 * pathToLogDir arg is full path to directory where desktop server log
 * will be created.
 */
class DesktopServerCommandLine : private CommandLine
{
public:
  DesktopServerCommandLine();
  virtual ~DesktopServerCommandLine();

  /**
   * Parses command line.
   * @param cmdLine command line to parse.
   * @throws Exception on fail.
   */
  virtual void parse(const CommandLineArgs *cmdArgs) throw(Exception);

  /**
   * Puts specified in command line path to log directory into output parameter.
   * @param [out] logDir output parameter for log directory.
   * @remark if parse method wasn't before this will return 0 as log dir.
   */
  void getLogDir(StringStorage *logDir);

  /**
   * Returns specified log level.
   * @return specified log level.
   */
  int getLogLevel();

  /**
   * Puts specified shared memory name into output parameter.
   */
  void getSharedMemName(StringStorage *shMemName);

public:
  const static TCHAR DESKTOP_SERVER_KEY[];

private:
  const static TCHAR LOG_DIR_KEY[];
  const static TCHAR LOG_LEVEL_KEY[];
  const static TCHAR SHARED_MEMORY_NAME_KEY[];
};

#endif
