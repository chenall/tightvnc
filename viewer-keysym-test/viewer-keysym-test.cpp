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

#include "KeySymTest.h"
#include "util/Exception.h"
#include <stdio.h>

int _tmain(int argc, TCHAR *argv[])
{
  if (argc != 3) {
    _ftprintf(stderr, _T("Not enough parameters\n"));
    return 1;
  }
  try {
    KeySymTest keySymTest(argv[1], argv[2]);
    keySymTest.run();
  } catch (Exception &e) {
    _ftprintf(stderr, _T("Error: %s\n"), e.getMessage());
    return 1;
  }
  return 0;
}
