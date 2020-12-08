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

#include "Screen.h"
#include "util/Exception.h"
#include "win-system/Environment.h"

Screen::Screen()
{
  update();
}

Screen::~Screen()
{
}

void Screen::update()
{
  BMI bmi;
  getBMI(&bmi, 0);

  fillPixelFormat(&bmi);
  fillScreenRect();
}

PixelFormat Screen::getPixelFormat()
{
  return m_pixelFormat;
}

Dimension Screen::getDesktopDimension()
{
  return Dimension(&m_virtDesktopRect);
}

Rect Screen::getDesktopRect()
{
  return m_virtDesktopRect;
}

void Screen::getBMI(BMI *bmi, HDC dc)
{
  HDC bitmapDC = dc;
  if (bitmapDC == 0) {
    bitmapDC = GetDC(0);
    if (bitmapDC == NULL) {
      throw Exception(_T("Can't get a bitmap dc"));
    }
  }

  memset(bmi, 0, sizeof(BMI));
  bmi->bmiHeader.biBitCount = 0;
  bmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);

  HBITMAP hbm;
  hbm = (HBITMAP)GetCurrentObject(bitmapDC, OBJ_BITMAP);
  if (GetDIBits(bitmapDC, hbm, 0, 0, NULL, (LPBITMAPINFO)bmi, DIB_RGB_COLORS) == 0) {
    StringStorage errMess;
    Environment::getErrStr(_T("Can't get a DIBits"), &errMess);
    DeleteObject(hbm);
    DeleteDC(bitmapDC);
    throw Exception(errMess.getString());
  }

  // The color table is filled only if it is used BI_BITFIELDS
  if (bmi->bmiHeader.biCompression == BI_BITFIELDS) {
    if (GetDIBits(bitmapDC, hbm, 0, 0, NULL, (LPBITMAPINFO)bmi, DIB_RGB_COLORS) == 0) {
      StringStorage errMess;
      Environment::getErrStr(_T("Can't get a DIBits"), &errMess);
      DeleteObject(hbm);
      DeleteDC(bitmapDC);
      throw Exception(errMess.getString());
    }
  }

  DeleteObject(hbm);
  if (dc == 0) {
    DeleteDC(bitmapDC);
  }
}

void Screen::fillPixelFormat(const BMI *bmi)
{
  memset(&m_pixelFormat, 0, sizeof(PixelFormat));

  m_pixelFormat.initBigEndianByNative();

  m_pixelFormat.bitsPerPixel = bmi->bmiHeader.biBitCount;

  if (bmi->bmiHeader.biCompression == BI_BITFIELDS) {
    m_pixelFormat.redShift   = findFirstBit(bmi->red);
    m_pixelFormat.greenShift = findFirstBit(bmi->green);
    m_pixelFormat.blueShift  = findFirstBit(bmi->blue);

    m_pixelFormat.redMax   = bmi->red    >> m_pixelFormat.redShift;
    m_pixelFormat.greenMax = bmi->green  >> m_pixelFormat.greenShift;
    m_pixelFormat.blueMax  = bmi->blue   >> m_pixelFormat.blueShift;

  } else {
    m_pixelFormat.bitsPerPixel = 32;
    m_pixelFormat.colorDepth = 24;
    m_pixelFormat.redMax = m_pixelFormat.greenMax = m_pixelFormat.blueMax = 0xff;
    m_pixelFormat.redShift   = 16;
    m_pixelFormat.greenShift = 8;
    m_pixelFormat.blueShift  = 0;
  }

  if (m_pixelFormat.bitsPerPixel == 32) {
    m_pixelFormat.colorDepth = 24;
  } else {
    m_pixelFormat.colorDepth = 16;
  }
}

int Screen::findFirstBit(const UINT32 bits)
{
  UINT32 b = bits;
  int shift;

  for (shift = 0; (shift < 32) && ((b & 1) == 0); shift++) {
    b >>= 1;
  }

  return shift;
}

void Screen::fillScreenRect()
{
  m_virtDesktopRect.left = GetSystemMetrics(SM_XVIRTUALSCREEN);
  m_virtDesktopRect.top = GetSystemMetrics(SM_YVIRTUALSCREEN);
  m_virtDesktopRect.setWidth(GetSystemMetrics(SM_CXVIRTUALSCREEN));
  m_virtDesktopRect.setHeight(GetSystemMetrics(SM_CYVIRTUALSCREEN));
}

size_t Screen::getVisibleMonitorCount()
{
  int monitorCount = GetSystemMetrics(SM_CMONITORS);
  // Why check for the result? Skip it.
  return (size_t)monitorCount;
}
