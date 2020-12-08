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

#ifndef _BITMAP_GRAPHICS_H_
#define _BITMAP_GRAPHICS_H_

#include "Graphics.h"
#include "Bitmap.h"

//
// Object that creates bitmap with specified size and allow to
// draw into in through Graphics class methods.
class BitmapGraphics : public Graphics
{
public:
  // Creates BitmapGraphics object with bitmap complatible with specified dc and
  // with specified size.
  BitmapGraphics(DeviceContext *complatibleDC, int width, int height);
  // Destroys bitmap and graphics object.
  virtual ~BitmapGraphics();

  // Returns target bitmap that we paint.
  Bitmap *getBitmap();

  void beginPaint();
  void endPaint();

protected:
  bool m_isPainting;
  Bitmap *m_bitmap;
  HGDIOBJ m_oldBitmap;
};

#endif
