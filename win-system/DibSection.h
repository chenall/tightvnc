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

#ifndef __DIBSECTION_H__
#define __DIBSECTION_H__

#include "util/CommonHeader.h"
#ifndef CAPTUREBLT
#define CAPTUREBLT          (DWORD)0x40000000 /* Include layered windows */
#endif
#include "win-system/Screen.h"

// This clas is a primitive wrapper to a DIB section.
class DibSection
{
public:
  // Note that if the compatibleWin doesn't specify or is zero the class will create an
  // DC for the entire screen. In this case an this object owner must provide
  // destructor calling from the same thread that called the constructor.
  // In default the created DC will used as a target DC for the blitting operations.
  // It may be changed many times later. Note that changed DC must be compatible with
  // the DIB section.
  DibSection(const PixelFormat *pf, const Dimension *dim, HWND compatibleWin = 0);
  virtual ~DibSection();

  // This function changes the target DC. In default target DC is a DC that has been
  // got from a compatible window on object creation. This function can be call many times.
  void setTargetDC(HDC targetDC);

  void *getBuffer();

  // This function copies a block of bits from a source DC (that has been used to create the
  // DIB section) to the DIB section.
  // Note that this function does not copy any transparent windows.
  // This function throwing an exception on a failure.
  void blitToDibSection(const Rect *rect);

  // This function copies a block of bits from a source DC (that has been used to create the
  // DIB section) to the DIB section.
  // Note that this function copies transparent windows too.
  // This function throwing an exception on a failure.
  void blitTransparentToDibSection(const Rect *rect);

  // This function copies a block of bits from the DIB section to the source DC
  // (that has been used to create the compatible DIB section).
  // Note that this function does not copy any transparent windows.
  // This function throwing an exception on a failure.
  void blitFromDibSection(const Rect *rect);

  // This function copies with strech a block of bits from the DIB section to the source DC
  // (that has been used to create the compatible DIB section).
  // Note that this function does not copy any transparent windows.
  // This function throwing an exception on a failure.
  void stretchFromDibSection(const Rect *srcRect, const Rect *dstRect);

private:
  // Opens a new DIB section.
  // If targetDC == 0 the function will use a current desktop DC.
  void openDIBSection(const PixelFormat *pf, const Dimension *dim, HWND compatibleWin);
  void closeDIBSection();
  void releaseTargetDC();

  // This function throwing an exception on a failure.
  void blitToDibSection(const Rect *rect, DWORD flags);

  // This function throwing an exception on a failure.
  void blitFromDibSection(const Rect *rect, DWORD flags);
  void stretchFromDibSection(const Rect *srcRect, const Rect *dstRect, DWORD flags);

  void setupBMIStruct(BITMAPINFO *pBmi, const PixelFormat *pf, const Dimension *dim);

  bool m_isOwnTargetDC;
  HDC m_targetDC;
  HDC m_memDC;
  HBITMAP m_hbmOld;
  HBITMAP m_hbmDIB;
  // Coordinates of the source dc can be negative.
  int m_srcOffsetX;
  int m_srcOffsetY;

  void *m_buffer;

  Screen m_screen;
};

#endif // __DIBSECTION_H__
