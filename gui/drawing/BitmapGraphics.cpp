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

#include "BitmapGraphics.h"

BitmapGraphics::BitmapGraphics(DeviceContext *complatibleDC, int width, int height)
: Graphics(complatibleDC), m_isPainting(false)
{
  m_bitmap = new Bitmap(complatibleDC->m_dc, width, height);
  m_dc = new DeviceContext(complatibleDC);
}

BitmapGraphics::~BitmapGraphics()
{
  delete m_bitmap;
  delete m_dc;
}

Bitmap *BitmapGraphics::getBitmap()
{
  return m_bitmap;
}

void BitmapGraphics::beginPaint()
{
  _ASSERT(!m_isPainting);

  m_isPainting = true;
  m_oldBitmap = m_dc->selectObject(m_bitmap->m_bitmap);
}

void BitmapGraphics::endPaint()
{
  _ASSERT(m_isPainting);

  m_isPainting = false;
  m_dc->selectObject(m_oldBitmap);
}
