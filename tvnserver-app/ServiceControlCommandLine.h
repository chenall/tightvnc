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

#ifndef _SERVICE_CONTROL_COMMAND_LINE_H_
#define _SERVICE_CONTROL_COMMAND_LINE_H_

#include "util/CommandLine.h"
#include "util/Exception.h"

/**
 * Service control application command line parser.
 * Supported keys are:
 *  -install, -reinstall, -remove, -start, -stop.
 * @remark only one allowed key can be specified.
 * @fixme no test for this class.
 */
class ServiceControlCommandLine : private CommandLine
{
public:
  static const TCHAR APPLICATION_KEY[];
  static const TCHAR INSTALL_SERVICE[];
  static const TCHAR REMOVE_SERVICE[];
  static const TCHAR REINSTALL_SERVICE[];
  static const TCHAR START_SERVICE[];
  static const TCHAR STOP_SERVICE[];
  static const TCHAR SILENT[];
  static const TCHAR DONT_ELEVATE[];

public:
  /**
   * Default constructor.
   */
  ServiceControlCommandLine();
  /**
   * Destructor.
   */
  virtual ~ServiceControlCommandLine();

  /**
   * Parses command line.
   * @param commandLine command line string to parse.
   * @throws Exception on parse error.
   */
  void parse(const CommandLineArgs *cmdArgs) throw(Exception);

  /**
   * Checks if key is specified.
   * @param key key to check.
   * @return true if key is specified, false otherwise.
   * @remark must be called only after call of parse method.
   */
  bool keySpecified(const TCHAR *key) const;

  bool installationRequested() const;
  bool removalRequested() const;
  bool reinstallRequested() const;
  bool startRequested() const;
  bool stopRequested() const;

  bool beSilent() const;
  bool dontElevate() const;
};

#endif
