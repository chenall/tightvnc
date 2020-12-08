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

#include "DynamicLibrary.h"

#include <crtdbg.h>

DynamicLibrary::DynamicLibrary(const TCHAR *filename)
: m_module(0)
{
  init(filename);
}

DynamicLibrary::DynamicLibrary()
: m_module(0)
{
}

DynamicLibrary::~DynamicLibrary()
{
  if (m_module != 0) {
    FreeLibrary(m_module);
  }
}

void DynamicLibrary::init(const TCHAR *filename)
{
  m_module = LoadLibrary(filename);

  if (m_module == 0) {
    StringStorage errMsg;

    errMsg.format(_T("%s library not found"), filename);

    throw Exception(errMsg.getString());
  }
}

FARPROC DynamicLibrary::getProcAddress(const char *procName)
{
  _ASSERT(m_module != 0);

  return ::GetProcAddress(m_module, procName);
}
