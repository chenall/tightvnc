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

#ifndef __WIN_PROCESS_COMMAND_LINE_H__
#define __WIN_PROCESS_COMMAND_LINE_H__

#include "WinCommandLineArgs.h"

// This class is a primitive command line parser
class WinProcessCommandLine
{
public:
  WinProcessCommandLine();
  virtual ~WinProcessCommandLine();

  // returns the number of arguments in command line
  // for example, program: help
  size_t getArgumentsCount();

  // returns the number of options in command line
  // for example, -V=123456
  size_t getOptionsCount();

  // returns the value of parameter by valName
  bool findOptionValue(const StringStorage valName, StringStorage* strOut);

  // returns the argument value with index
  bool getArgument(size_t index, StringStorage* strOut);

  // returns the option value with index
  bool getOption(size_t index, StringStorage* strOut);

protected:
  WinCommandLineArgs *m_wcla;

  vector<StringStorage> m_strParam;
  vector<pair<StringStorage, StringStorage>> m_strParams;

private:
  void optionParser(StringStorage *out);

};

#endif
