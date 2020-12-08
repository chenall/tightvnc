// Copyright (C) 2011,2012 GlavSoft LLC.
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

#ifndef _STANDARD_PIXEL_FORMAT_FACTORY_
#define _STANDARD_PIXEL_FORMAT_FACTORY_

#include "rfb/PixelFormat.h"

class StandardPixelFormatFactory
{
public:
  // specifies 16777216 colors, 8bit per Red, 8bit per Green, 8bit per Blue
  static PixelFormat create32bppPixelFormat();
  
  // specifies 65536 colors, 5bit per Red, 6bit per Green, 5bit per Blue
  static PixelFormat create16bppPixelFormat();

  // specifies 256 colors, 2bit per Blue, 3bit per Green & Red
  static PixelFormat create8bppPixelFormat();

  // specifies 64 colors, 2bit per Red, Green & Blue
  static PixelFormat create6bppPixelFormat();

  // specifies 8 colors, 1bit per Red, Green & Blue
  static PixelFormat create3bppPixelFormat();
};

#endif
