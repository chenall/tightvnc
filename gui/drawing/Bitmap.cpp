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

#include "Bitmap.h"

Bitmap::Bitmap(int width, int height)
: m_bitmap(NULL)
{
  // Prepare buffer
  int bpp = 32;
  size_t size = width * height * (bpp / 8);
  std::vector<unsigned char> bits(size);
  if (width != 0 && height != 0) {
    memset(&bits.front(), 0, size);
    // Create bitmap handle
    m_bitmap = CreateBitmap(width, height, 1, bpp, &bits.front());
  }
}

Bitmap::Bitmap(HDC dc, int width, int height)
{
  m_bitmap = CreateCompatibleBitmap(dc, width, height);
}

Bitmap::Bitmap(HBITMAP bitmap)
: m_bitmap(bitmap)
{
}

Bitmap::~Bitmap()
{
  if (m_bitmap != NULL) {
    DeleteObject(m_bitmap);
  }
}

int Bitmap::getWidth() const
{
  BITMAP bitmap;
  if (GetObject(m_bitmap, sizeof(BITMAP), &bitmap) == 0) {
    return 0;
  }
  return bitmap.bmWidth;
}

int Bitmap::getHeight() const
{
  BITMAP bitmap;
  if (GetObject(m_bitmap, sizeof(BITMAP), &bitmap) == 0) {
    return 0;
  }
  return bitmap.bmHeight;
}
