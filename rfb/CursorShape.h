// Copyright (C) 2008,2009,2010,2011,2012 GlavSoft LLC.
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

#ifndef __CURSORSHAPE_H__
#define __CURSORSHAPE_H__

#include "FrameBuffer.h"
#include "region/Point.h"

class CursorShape
{
public:
  CursorShape(): m_mask(0) {}
  ~CursorShape();

  bool clone(const CursorShape *srcCursorShape);

  bool setDimension(const Dimension *newDim);
  Dimension getDimension() const { return m_pixels.getDimension(); }

  bool setPixelFormat(const PixelFormat *pixelFormat);
  PixelFormat getPixelFormat() const { return m_pixels.getPixelFormat(); }

  // This function set both PixelFormat and Dimension
  bool setProperties(const Dimension *newDim, const PixelFormat *pixelFormat);

  const FrameBuffer *getPixels() const { return &m_pixels; }
  int getPixelsSize() const { return m_pixels.getBufferSize(); }
  const char *getMask() const { return m_mask.empty() ? 0 : &m_mask.front(); }
  void assignMaskFromRfb(const char *srcMask);
  void assignMaskFromWindows(const char *srcMask);
  int getMaskSize() const;
  int getMaskWidthInBytes() const;

  void setHotSpot(int x, int y) { m_hotSpot.x = x; m_hotSpot.y = y; }
  Point getHotSpot() const { return m_hotSpot; }

  // Resets the cursor shape to empty state (zero dimension and hot spot
  // and empty masks). But the pixel format saves the same.
  void resetToEmpty();

private:
  bool resizeBuffer();

  FrameBuffer m_pixels;
  std::vector<char> m_mask;
  Point m_hotSpot;
};

#endif // __CURSORSHAPE_H__
