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

#include "ServiceControlCommandLine.h"

const TCHAR ServiceControlCommandLine::INSTALL_SERVICE[]   = _T("-install");
const TCHAR ServiceControlCommandLine::REMOVE_SERVICE[]    = _T("-remove");
const TCHAR ServiceControlCommandLine::REINSTALL_SERVICE[] = _T("-reinstall");
const TCHAR ServiceControlCommandLine::START_SERVICE[]     = _T("-start");
const TCHAR ServiceControlCommandLine::STOP_SERVICE[]      = _T("-stop");

const TCHAR ServiceControlCommandLine::DONT_ELEVATE[]            = _T("-dontelevate");
const TCHAR ServiceControlCommandLine::SILENT[]            = _T("-silent");

ServiceControlCommandLine::ServiceControlCommandLine()
{
}

ServiceControlCommandLine::~ServiceControlCommandLine()
{
}

void ServiceControlCommandLine::parse(const CommandLineArgs *cmdArgs)
{
  CommandLineFormat format[] = {
    { INSTALL_SERVICE, NO_ARG },
    { REMOVE_SERVICE, NO_ARG },
    { REINSTALL_SERVICE, NO_ARG },
    { START_SERVICE, NO_ARG },
    { STOP_SERVICE, NO_ARG },
    { SILENT, NO_ARG },
    { DONT_ELEVATE, NO_ARG }
  };

  if (!CommandLine::parse(format,
                          sizeof(format) / sizeof(CommandLineFormat),
                          cmdArgs)) {
    throw Exception(_T("invalid command line"));
  } // if cannot parse it.

  // Check additional rules.

  if (keySpecified(SILENT)) {
    if (m_foundKeys.size() != (dontElevate() ? 3 : 2)) {
      throw Exception(_T("-silent key can be used only when one command specified"));
    }
  } else if (m_foundKeys.size() != (dontElevate() ? 2 : 1)) {
    throw Exception(_T("only one service command can be specified"));
  }
}

bool ServiceControlCommandLine::keySpecified(const TCHAR *key) const
{
  return optionSpecified(key);
}

bool ServiceControlCommandLine::installationRequested() const
{
  return keySpecified(INSTALL_SERVICE);
}

bool ServiceControlCommandLine::removalRequested() const
{
  return keySpecified(REMOVE_SERVICE);
}

bool ServiceControlCommandLine::reinstallRequested() const
{
  return keySpecified(REINSTALL_SERVICE);
}

bool ServiceControlCommandLine::startRequested() const
{
  return keySpecified(START_SERVICE);
}

bool ServiceControlCommandLine::stopRequested() const
{
  return keySpecified(STOP_SERVICE);
}

bool ServiceControlCommandLine::beSilent() const
{
  return keySpecified(SILENT);
}

bool ServiceControlCommandLine::dontElevate() const
{
  return keySpecified(DONT_ELEVATE);
}
