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

#ifndef __SCREEN_H__
#define __SCREEN_H__

#include "util/CommonHeader.h"
#include "rfb/PixelFormat.h"
#include "region/Rect.h"
#include "region/Dimension.h"

// This class get info for a windows desktop.
class Screen
{
public:
  Screen();
  ~Screen();

  void update();

  // Returns a PixelFormat that was at latest call of the
  // update() function.
  PixelFormat getPixelFormat();

  // Returns a desktop dimension that was at latest call of the
  // update() function.
  // Desktop dimension is a dimension of windows virtual desktop including
  // all monitors.
  Dimension getDesktopDimension();

  // Returns a rectangle that was at latest call of the
  // update() function.
  // The rectangle is a rectangle of windows virtual desktop including
  // all monitors (coordinates can be negative).
  Rect getDesktopRect();

  // This structure can be used by user code.
  struct BMI
  {
    BITMAPINFOHEADER bmiHeader;
    UINT32 red;
    UINT32 green;
    UINT32 blue;
  };

  struct Palette8bitBMI
  {
    BITMAPINFOHEADER bmiHeader;
    RGBQUAD rgbQuad[256];
  };

  // Fills the BMI structure. If dc == 0 the getBMI() function will
  // use a current desktop dc.
  void getBMI(BMI *bmi, HDC dc);

  // Windows contain both visible and invisible pseudo-monitors
  // that are associated with mirroring drivers.
  // The function returns only visible monitor count.
  size_t getVisibleMonitorCount();

private:
  void fillPixelFormat(const BMI *bmi);
  // Find position of first true bit
  static inline int findFirstBit(const UINT32 bits);

  void fillScreenRect();

  PixelFormat m_pixelFormat;
  Rect m_virtDesktopRect;
};

#endif // __SCREEN_H__
