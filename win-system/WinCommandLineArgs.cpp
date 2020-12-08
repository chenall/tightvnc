// Copyright (C) 2010,2011,2012 GlavSoft LLC.
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

#include "WinCommandLineArgs.h"
#include "util/Exception.h"
#include "util/UnicodeStringStorage.h"

WinCommandLineArgs::WinCommandLineArgs(const TCHAR *cmdLineInWinFormat)
{
  UnicodeStringStorage uniCmdLine(&StringStorage(cmdLineInWinFormat));
  size_t cmdLen = uniCmdLine.getLength();
  if (cmdLen > 0) {
    int nArgs;
    LPWSTR *argList = CommandLineToArgvW(uniCmdLine.getString(), &nArgs);
    if(argList == 0) {
      throw Exception(_T("Invalid command line"));
    }
    for(int i = 0; i < nArgs; i++) {
      UnicodeStringStorage uniArg(argList[i]);
      StringStorage arg;
      uniArg.toStringStorage(&arg);
      if (arg.getLength() > 0) {
        m_args.push_back(arg);
      }
    }

    LocalFree(argList);
  }
}

WinCommandLineArgs::~WinCommandLineArgs()
{
}
