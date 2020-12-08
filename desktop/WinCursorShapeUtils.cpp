// Copyright (C) 2012 GlavSoft LLC.
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

#include "WinCursorShapeUtils.h"

WinCursorShapeUtils::WinCursorShapeUtils()
{
}

WinCursorShapeUtils::~WinCursorShapeUtils()
{
}

void WinCursorShapeUtils::winMonoShapeToRfb(const FrameBuffer *pixels,
                                            char *maskAND, char *maskXOR)
{
  char *pixelsBuffer = (char *)pixels->getBuffer();
  char *pixel;
  int pixelSize = pixels->getBytesPerPixel();
  int pixelCount = pixels->getBufferSize() / pixelSize;

  int fbWidth = pixels->getDimension().width;
  int fbHeight = pixels->getDimension().height;
  int widthInBytes = ((fbWidth + 15) / 16) * 2;

  for (int iRow = 0; iRow < fbHeight; iRow++) {
    for (int iCol = 0; iCol < fbWidth; iCol++) {
      pixel = pixelsBuffer + (iRow * fbWidth + iCol) * pixelSize;

      char byteAnd = maskAND[iRow * widthInBytes + iCol / 8];
      char byteXor = maskXOR[iRow * widthInBytes + iCol / 8];

      bool maskANDBit = testBit(byteAnd, iCol % 8);
      bool maskXORBit = testBit(byteXor, iCol % 8);

      if (!maskANDBit && !maskXORBit) { // Black dot
        memset(pixel, 0, pixelSize);
      } else if (!maskANDBit && maskXORBit) { // White dot
        memset(pixel, 0xff, pixelSize);
      } else if (maskANDBit && maskXORBit) { // Inverted (as black dot)
        memset(pixel, 0, pixelSize);
      }
    }
  }

  inverse(maskAND, widthInBytes * fbHeight);
  for (int i = 0; i < widthInBytes * fbHeight; i++) {
    *(maskAND + i) |= *(maskXOR + i);
  }
}

bool WinCursorShapeUtils::testBit(char byte, int index)
{
  char dummy = 128 >> index;
  return (dummy & byte) != 0;
}

void WinCursorShapeUtils::fixAlphaChannel(const FrameBuffer *pixels,
                                               char *maskAND, bool isInversedAlpha)
{
  PixelFormat pf = pixels->getPixelFormat();
  if (pf.bitsPerPixel != 32) {
    return;
  }
  UINT32 alphaMax = getAlphaMask(&pf);
  UINT16 alphaShift = 0;
  for (alphaShift = 0; alphaShift < 32 && (alphaMax % 2) == 0 ; alphaShift++) {
    alphaMax = alphaMax >> 1;
  }
  if (alphaShift > 24) { // We don't know how do it
    return;
  }

  UINT32 *pixelBuffer = (UINT32 *)pixels->getBuffer();
  int pixelSize = pixels->getBytesPerPixel();

  int fbWidth = pixels->getDimension().width;
  int fbHeight = pixels->getDimension().height;
  int widthInBytes = ((fbWidth + 15) / 16) * 2;

  for (int iRow = 0; iRow < fbHeight; iRow++) {
    for (int iCol = 0; iCol < fbWidth; iCol++) {
      UINT32 *pixel = &pixelBuffer[iRow * fbWidth + iCol];
      bool transparent = ((*pixel >> alphaShift) & alphaMax) < 128;
      if (isInversedAlpha) {
        transparent = !transparent;
      }
      if (transparent) {
        char *byteAnd = &maskAND[iRow * widthInBytes + iCol / 8];
        unsigned char curBit = 128 >> iCol % 8;
        curBit = ~curBit;
        *byteAnd = *byteAnd & curBit;
      }
    }
  }
}

UINT32 WinCursorShapeUtils::getAlphaMask(const PixelFormat *pf)
{
  if (pf->bitsPerPixel == 32) {
    UINT32 alphaMax = pf->redMax << pf->redShift |
                      pf->greenMax << pf->greenShift |
                      pf->blueMax << pf->blueShift;
    return ~alphaMax;
  } else {
    return 0;
  }
}

void WinCursorShapeUtils::inverse(char *bits, int count)
{
  for (int i = 0; i < count; i++, bits++) {
    *bits = ~*bits;
  }
}
