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

#include "StandardPixelFormatFactory.h"


PixelFormat StandardPixelFormatFactory::create32bppPixelFormat()
{
  PixelFormat pixelFormat;
  pixelFormat.initBigEndianByNative();
  pixelFormat.bitsPerPixel = 32;
  pixelFormat.blueMax = 255;
  pixelFormat.blueShift = 0;
  pixelFormat.greenMax = 255;
  pixelFormat.greenShift = 8;
  pixelFormat.redMax = 255;
  pixelFormat.redShift = 16;
  pixelFormat.colorDepth = 24;
  return pixelFormat;
}

PixelFormat StandardPixelFormatFactory::create16bppPixelFormat()
{
  PixelFormat pixelFormat;
  pixelFormat.initBigEndianByNative();
  pixelFormat.bitsPerPixel = 16;
  pixelFormat.blueMax = 31;
  pixelFormat.blueShift = 0;
  pixelFormat.greenMax = 63;
  pixelFormat.greenShift = 5;
  pixelFormat.redMax = 31;
  pixelFormat.redShift = 11;
  pixelFormat.colorDepth = 16;
  return pixelFormat;
}

PixelFormat StandardPixelFormatFactory::create8bppPixelFormat()
{
  PixelFormat pixelFormat;
  pixelFormat.initBigEndianByNative();
  pixelFormat.bitsPerPixel = 8;
  pixelFormat.redMax = 7;
  pixelFormat.redShift = 0;
  pixelFormat.greenMax = 7;
  pixelFormat.greenShift = 3;
  pixelFormat.blueMax = 3;
  pixelFormat.blueShift = 6;
  pixelFormat.colorDepth = 8;
  return pixelFormat;
}

PixelFormat StandardPixelFormatFactory::create6bppPixelFormat()
{
  PixelFormat pixelFormat;
  pixelFormat.initBigEndianByNative();
  pixelFormat.bitsPerPixel = 8;
  pixelFormat.blueMax = 3;
  pixelFormat.blueShift = 0;
  pixelFormat.greenMax = 3;
  pixelFormat.greenShift = 2;
  pixelFormat.redMax = 3;
  pixelFormat.redShift = 4;
  pixelFormat.colorDepth = 6;
  return pixelFormat;
}

PixelFormat StandardPixelFormatFactory::create3bppPixelFormat()
{
  PixelFormat pixelFormat;
  pixelFormat.initBigEndianByNative();
  pixelFormat.bitsPerPixel = 8;
  pixelFormat.blueMax = 1;
  pixelFormat.blueShift = 0;
  pixelFormat.greenMax = 1;
  pixelFormat.greenShift = 1;
  pixelFormat.redMax = 1;
  pixelFormat.redShift = 2;
  pixelFormat.colorDepth = 3;
  return pixelFormat;
}
