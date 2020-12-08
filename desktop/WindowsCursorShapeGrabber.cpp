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

#include "WindowsCursorShapeGrabber.h"
#include "WinCursorShapeUtils.h"

WindowsCursorShapeGrabber::WindowsCursorShapeGrabber(void)
: m_lastHCursor(0)
{
}

WindowsCursorShapeGrabber::~WindowsCursorShapeGrabber(void)
{
}

bool WindowsCursorShapeGrabber::isCursorShapeChanged()
{
  HCURSOR hCursor = getHCursor();
  if (hCursor == m_lastHCursor) {
    return false;
  }
  m_lastHCursor = hCursor;

  return true;
}

bool WindowsCursorShapeGrabber::grab(const PixelFormat *pixelFormat)
{
  return grabPixels(pixelFormat);
}

bool WindowsCursorShapeGrabber::grabPixels(const PixelFormat *pixelFormat)
{
  HCURSOR hCursor = getHCursor();
  if (hCursor == 0) {
    return false;
  }
  m_lastHCursor = hCursor;

  // Get bitmap mask
  ICONINFO iconInfo;
  if (!GetIconInfo(hCursor, &iconInfo)) {
    return false;
  }

  if (iconInfo.hbmMask == NULL) {
    return false;
  }

  bool isColorShape = (iconInfo.hbmColor != NULL);

  BITMAP bmMask;
  if (!GetObject(iconInfo.hbmMask, sizeof(BITMAP), (LPVOID)&bmMask)) {
    DeleteObject(iconInfo.hbmMask);
    if (isColorShape) {
      DeleteObject(iconInfo.hbmColor);
    }
    return false;
  }

  if (bmMask.bmPlanes != 1 || bmMask.bmBitsPixel != 1) {
    DeleteObject(iconInfo.hbmMask);
    if (isColorShape) {
      DeleteObject(iconInfo.hbmColor);
    }
    return false;
  }

  m_cursorShape.setHotSpot(iconInfo.xHotspot, iconInfo.yHotspot);

  int width = bmMask.bmWidth;
  int height = isColorShape ? bmMask.bmHeight : bmMask.bmHeight/2;
  int widthBytes = bmMask.bmWidthBytes;

  const FrameBuffer *pixels= m_cursorShape.getPixels();

  m_cursorShape.setProperties(&Dimension(width, height), pixelFormat);

  std::vector<char> maskBuff(widthBytes * bmMask.bmHeight);
  if (maskBuff.empty()) {
    DeleteObject(iconInfo.hbmMask);
    if (isColorShape) {
      DeleteObject(iconInfo.hbmColor);
    }
    return true;
  }
  char *mask = &maskBuff.front();

  // FIXME: Use try-catch block to escape code duplication
  // and free resources on an error.

  bool result = GetBitmapBits(iconInfo.hbmMask,
                              widthBytes * bmMask.bmHeight,
                              mask) != 0;

  DeleteObject(iconInfo.hbmMask);
  if (isColorShape) {
    DeleteObject(iconInfo.hbmColor);
  }
  if (!result) {
    return false;
  }

  // Get cursor pixels
  HDC screenDC = GetDC(0);
  if (screenDC == NULL) {
    return false;
  }

  Screen::BMI bmi;
  try {
    m_screen.getBMI(&bmi, screenDC);
  } catch (...) {
    return false;
  }

  bmi.bmiHeader.biBitCount = pixelFormat->bitsPerPixel;
  bmi.bmiHeader.biWidth = width;
  bmi.bmiHeader.biHeight = -height;
  bmi.bmiHeader.biCompression = BI_BITFIELDS;
  bmi.red   = pixelFormat->redMax   << pixelFormat->redShift;
  bmi.green = pixelFormat->greenMax << pixelFormat->greenShift;
  bmi.blue  = pixelFormat->blueMax  << pixelFormat->blueShift;

  HDC destDC = CreateCompatibleDC(NULL);
  if (destDC == NULL) {
    DeleteDC(screenDC);
    return false;
  }

  void *buffer;
  HBITMAP hbmDIB = CreateDIBSection(destDC, (BITMAPINFO *) &bmi, DIB_RGB_COLORS, &buffer, NULL, NULL);
  if (hbmDIB == 0) {
    DeleteDC(destDC);
    DeleteDC(screenDC);
    return false;
  }

  HBITMAP hbmOld = (HBITMAP)SelectObject(destDC, hbmDIB);

  result = DrawIconEx(destDC, 0, 0, hCursor, 0, 0, 0, NULL, DI_IMAGE) != 0;

  memcpy(pixels->getBuffer(), buffer, pixels->getBufferSize());

  if (!isColorShape) {
    WinCursorShapeUtils::winMonoShapeToRfb(pixels, mask, mask + widthBytes * height);
  } else {
    if (pixels->getBitsPerPixel() == 32) {
      if (WinCursorShapeUtils::winColorShapeToRfb<UINT32>(pixels, mask)) {
        // If the alpha channel is presented.
        WinCursorShapeUtils::fixAlphaChannel(pixels, mask, false);
      }
    } else if (pixels->getBitsPerPixel() == 16) {
      WinCursorShapeUtils::winColorShapeToRfb<UINT16>(pixels, mask);
    }
  }

  m_cursorShape.assignMaskFromWindows(mask);

  SelectObject(destDC, hbmOld);
  DeleteObject(hbmDIB);
  DeleteDC(destDC);
  DeleteDC(screenDC);

  return result;
}

HCURSOR WindowsCursorShapeGrabber::getHCursor()
{
  CURSORINFO cursorInfo;
  cursorInfo.cbSize = sizeof(CURSORINFO);

  if (GetCursorInfo(&cursorInfo) == 0) {
    return false;
  }

  return cursorInfo.hCursor;
}
