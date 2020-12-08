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

#ifndef __WINCURSORSHAPEUTILS_H__
#define __WINCURSORSHAPEUTILS_H__

#include "rfb/FrameBuffer.h"

// This class provides some functions to work with windows cursor shape data.
class WinCursorShapeUtils
{
public:
  WinCursorShapeUtils();
  virtual ~WinCursorShapeUtils();


  // Builds rfb mask AND by alpha channel of given pixels.
  // If isInversedAlpha is false the pixel supposes by transparent if alpha value less than 128.
  // If isInversedAlpha is true the pixel supposes by transparent if alpha value greater than 128.
  static void fixAlphaChannel(const FrameBuffer *pixels, char *maskAND, bool isInversedAlpha);

  // This function combines windows cursor mask and image and convert
  // theirs to rfb format. This function uses for monochrome cursor image.
  static void winMonoShapeToRfb(const FrameBuffer *pixels, char *maskAND, char *maskXOR);

  //   This function combines windows the cursor mask and image and convert
  // theirs to rfb format. This function uses for 16 or 24 bit color cursor
  // image.
  //   Also, this function determines whether image contains the alhpa channel
  // and returns true in this case.
  template< typename T >
  static bool winColorShapeToRfb(const FrameBuffer *pixels, char *maskAND);

private:
  // Inverts bit array with the "not" operator.
  static void inverse(char *bits, int count);

  // Returns true if a bit on the index place is true. High-order bit has zero index.
  static bool testBit(char byte, int index);

  static UINT32 getAlphaMask(const PixelFormat *pf);
};

template< typename T >
static bool WinCursorShapeUtils::winColorShapeToRfb(const FrameBuffer *pixels,
                                                    char *maskAND)
{
  char *pixelsBuffer = (char *)pixels->getBuffer();
  PixelFormat pf = pixels->getPixelFormat();
  T *pixel;

  int fbWidth = pixels->getDimension().width;
  int fbHeight = pixels->getDimension().height;
  int widthInBytes = ((fbWidth + 15) / 16) * 2;

  bool hasAlphaChannel = false;
  UINT32 alphaMask = getAlphaMask(&pf);

  for (int iRow = 0; iRow < fbHeight; iRow++) {
    for (int iCol = 0; iCol < fbWidth; iCol++) {
      pixel = (T *)pixelsBuffer + iRow * fbWidth + iCol;

      size_t iMaskAnd = iRow * widthInBytes + iCol / 8;
      char byteAnd = maskAND[iMaskAnd];
      bool maskANDBit = testBit(byteAnd, iCol % 8);

      if (!maskANDBit) { // *pixel = *pixel
        // Set current mask bit to true
        maskAND[iMaskAnd] = maskAND[iMaskAnd] | 128 >> (iCol % 8);
      } else if ((*pixel >> pf.redShift & pf.redMax) == 0 &&
                 (*pixel >> pf.greenShift & pf.greenMax) == 0 &&
                 (*pixel >> pf.blueShift & pf.blueMax) == 0) { // Transparent dot
        maskAND[iMaskAnd] = maskAND[iMaskAnd] & ~(128 >> (iCol % 8));
      } else { // Inverted (as black dot)
        // Set current mask bit to true
        maskAND[iMaskAnd] = maskAND[iMaskAnd] | 128 >> (iCol % 8);
        // Set pixel to black with the alpa channel preserving
        *pixel &= ~((T)pf.redMax << pf.redShift);
        *pixel &= ~((T)pf.greenMax << pf.greenShift);
        *pixel &= ~((T)pf.blueMax << pf.blueShift);
      }
      // Test for the alpha channel presence
      hasAlphaChannel = hasAlphaChannel || (*pixel & alphaMask) != 0;
    }
  }
  return hasAlphaChannel;
}

#endif // __WINCURSORSHAPEUTILS_H__
