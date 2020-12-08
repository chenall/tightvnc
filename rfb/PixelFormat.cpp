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

#include "PixelFormat.h"

#include <string.h>

PixelFormat::PixelFormat()
{
  memset(this, 0, sizeof(PixelFormat));
}

void PixelFormat::initBigEndianByNative()
{
  union {
    char test;
    int i;
  } testBigEndian;
  testBigEndian.i = 1;
  bigEndian = (testBigEndian.test == 0);
}

bool PixelFormat::isEqualTo(const PixelFormat *pf) const {
  return bitsPerPixel == pf->bitsPerPixel &&
         colorDepth   == pf->colorDepth &&
         redMax       == pf->redMax &&
         greenMax     == pf->greenMax &&
         blueMax      == pf->blueMax &&
         redShift     == pf->redShift &&
         greenShift   == pf->greenShift &&
         blueShift    == pf->blueShift &&
         bigEndian    == pf->bigEndian;
}
