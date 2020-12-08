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

#ifndef _NAMING_DEFS_H_
#define _NAMING_DEFS_H_

#include "util/CommonHeader.h"

class ProductNames
{
public:
  static const TCHAR PRODUCT_NAME[];
  static const TCHAR VIEWER_PRODUCT_NAME[];
};

class RegistryPaths
{
public:
  static const TCHAR VIEWER_PATH[];
};

class LogNames
{
public:
  static const TCHAR VIEWER_LOG_FILE_STUB_NAME[];
  static const TCHAR LOG_DIR_NAME[];
};


class ApplicationNames
{
public:
  static const TCHAR WINDOW_CLASS_NAME[];
};

class WindowNames
{
public:
  static const TCHAR TVN_WINDOW_CLASS_NAME[];
  static const TCHAR TVN_WINDOW_TITLE_NAME[];
  static const TCHAR TVN_SUB_WINDOW_TITLE_NAME[];
};

#endif
