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

#include "DesktopServerCommandLine.h"

#include "util/StringParser.h"

const TCHAR DesktopServerCommandLine::DESKTOP_SERVER_KEY[] = _T("-desktopserver");
const TCHAR DesktopServerCommandLine::LOG_DIR_KEY[] = _T("-logdir");
const TCHAR DesktopServerCommandLine::LOG_LEVEL_KEY[] = _T("-loglevel");
const TCHAR DesktopServerCommandLine::SHARED_MEMORY_NAME_KEY[] = _T("-shmemname");

DesktopServerCommandLine::DesktopServerCommandLine()
{
}

DesktopServerCommandLine::~DesktopServerCommandLine()
{
}

void DesktopServerCommandLine::parse(const CommandLineArgs *cmdArgs)
{
  CommandLineFormat format[] = {
    { DESKTOP_SERVER_KEY, NO_ARG },
    { LOG_DIR_KEY, NEEDS_ARG },
    { LOG_LEVEL_KEY, NEEDS_ARG },
    { SHARED_MEMORY_NAME_KEY, NEEDS_ARG } };

  if (!CommandLine::parse(format, sizeof(format) /
                                  sizeof(CommandLineFormat), cmdArgs) ||
      !optionSpecified(DESKTOP_SERVER_KEY) || !optionSpecified(LOG_DIR_KEY) ||
      !optionSpecified(LOG_LEVEL_KEY) ||
      !optionSpecified(SHARED_MEMORY_NAME_KEY)) {
    throw Exception(_T("Command line is invalid"));
  }
}

void DesktopServerCommandLine::getLogDir(StringStorage *logDir)
{
  if (!optionSpecified(LOG_DIR_KEY, logDir)) {
    _ASSERT(FALSE);

    logDir->setString(0);
  }
}

int DesktopServerCommandLine::getLogLevel()
{
  StringStorage logLevelKeyArg;

  if (!optionSpecified(LOG_LEVEL_KEY, &logLevelKeyArg)) {
    _ASSERT(FALSE);
  }

  int ret;

  if (!StringParser::parseInt(logLevelKeyArg.getString(), &ret)) {
    _ASSERT(FALSE);
  }

  return ret;
}

void DesktopServerCommandLine::getSharedMemName(StringStorage *shMemName)
{
  if (!optionSpecified(SHARED_MEMORY_NAME_KEY, shMemName)) {
    _ASSERT(FALSE);

    shMemName->setString(0);
  }
}
