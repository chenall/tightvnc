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

#ifndef _LIBRARY_H_
#define _LIBRARY_H_

#include "util/CommonHeader.h"

#include "util/Exception.h"

/**
Dynamic library class.
*/
class DynamicLibrary
{
public:
  /**
  Load dynamic library with specified filename.
  @param filename path to library file.
  @throws Exception on error.
  */
  DynamicLibrary(const TCHAR *filename) throw(Exception);
  DynamicLibrary();
  virtual ~DynamicLibrary();

  // Use the init() function after default constructor calling to load
  // a library before the getProcAddress() function calling.
  void init(const TCHAR *filename);

  /**
  Gets procedure address.
  @param procName procedure name.
  @return address of procedure or 0 if failed.
  */
  FARPROC getProcAddress(const char *procName);

protected:
  HMODULE m_module;
};

#endif
