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

#include "Graphics.h"

Graphics::Graphics(DeviceContext *dc)
: m_dc(dc)
{
}

Graphics::~Graphics()
{
}

void Graphics::setBkMode(bool transparent)
{
  SetBkMode(m_dc->m_dc, transparent ? TRANSPARENT : OPAQUE);
}

void Graphics::setTextColor(COLORREF color)
{
  SetTextColor(m_dc->m_dc, color);
}

void Graphics::setBkColor(COLORREF color)
{
  SetBkColor(m_dc->m_dc, color);
}

void Graphics::setBrush(const Brush *brush)
{
  HGDIOBJ object = (brush != 0) ? brush->m_brush : 0;
  m_dc->selectObject(object);
}

void Graphics::setPen(const Pen *pen)
{
  HGDIOBJ object = (pen != 0) ? pen->m_pen : 0;
  m_dc->selectObject(object);
}

void Graphics::moveTo(int x, int y)
{
  MoveToEx(m_dc->m_dc, x, y, NULL);
}

void Graphics::lineTo(int x, int y)
{
  LineTo(m_dc->m_dc, x, y);
}

void Graphics::fillRect(int l, int t, int r, int b, const Brush *brush)
{
  RECT rect;

  rect.top = t;
  rect.left = l;
  rect.bottom = b;
  rect.right = r;

  FillRect(m_dc->m_dc, &rect, brush->m_brush);
}

void Graphics::ellipse(int l, int t, int r, int b)
{
  Ellipse(m_dc->m_dc, l, t, r, b);
}

void Graphics::rectangle(int l, int t, int r, int b)
{
  Rectangle(m_dc->m_dc, l, t, r, b);
}

void Graphics::drawBitmap(const Bitmap *bitmap, int x, int y, int w, int h)
{
  DeviceContext memDC(m_dc);

  HGDIOBJ oldBitmap = memDC.selectObject(bitmap->m_bitmap);

  BitBlt(m_dc->m_dc, x, y, w, h, memDC.m_dc, 0, 0, SRCCOPY);

  memDC.selectObject(oldBitmap);
}

void Graphics::drawText(const TCHAR *text, int cchText, RECT *rect, UINT format)
{
  DrawText(m_dc->m_dc, text, cchText, rect, format);
}
