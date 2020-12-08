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

#ifndef __DIBFRAMEBUFFER_H__
#define __DIBFRAMEBUFFER_H__

#include "rfb/FrameBuffer.h"
#include "win-system/DibSection.h"

// This class is a wrapper for a FramBuffer and a DIB section.
// It changes DIB section proerties by oneself according to FrameBuffer
// properties (such as width, height and PixelFormat)
class DibFrameBuffer : public FrameBuffer
{
public:
  DibFrameBuffer();
  virtual ~DibFrameBuffer();

  virtual void setColor(UINT8 reg, UINT8 green, UINT8 blue);
  virtual void fillRect(const Rect *dstRect, UINT32 color);

  virtual bool isEqualTo(const FrameBuffer *frameBuffer);

  virtual bool copyFrom(const Rect *dstRect, const FrameBuffer *srcFrameBuffer,
                        int srcX, int srcY);
  virtual bool copyFrom(const FrameBuffer *srcFrameBuffer,
                        int srcX, int srcY);
  virtual bool overlay(const Rect *dstRect, const FrameBuffer *srcFrameBuffer,
                       int srcX, int srcY, const char *andMask);
  virtual void move(const Rect *dstRect, const int srcX, const int srcY);
  virtual bool cmpFrom(const Rect *dstRect, const FrameBuffer *srcFrameBuffer,
                       const int srcX, const int srcY);

  virtual inline Dimension getDimension() const;

  virtual inline PixelFormat getPixelFormat() const;

  // This function must uses instead of function that can change the FrameBuffer properties
  // compatibleWindow - is hwnd of a window that will be used to create a compatible DC for
  // the DIB section. Also, a DC of this window will be used as default for the
  // blitting operations. The window or DC for blitting operations can be changed many times during
  // a session of the DIB section later.
  // The compatibleWindow handle can be zero then the function will take a DC of entire desktop.
  // Note that other function that can change properties will throw Exception().
  virtual void setProperties(const Dimension *newDim,
    const PixelFormat *pixelFormat, HWND compatibleWindow);

  // This function changes the target DC. In default target DC is a DC that has been
  // got from a compatible window on object creation. This function can be call many times.
  void setTargetDC(HDC targetDC);

  virtual UINT8 getBitsPerPixel() const;

  virtual UINT8 getBytesPerPixel() const;

  virtual inline void *getBuffer() const;

  virtual void *getBufferPtr(int x, int y) const;

  virtual inline int getBufferSize() const;
  virtual inline int getBytesPerRow() const;

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
  // This section to reduce access to some function that have been inherited from the
  // FrameBuffer class and can't to be use in here. Also, if user code will to try
  // use this functions from a base class its will throw Exception.
  virtual bool assignProperties(const FrameBuffer *srcFrameBuffer);
  virtual bool clone(const FrameBuffer *srcFrameBuffer);
  virtual bool setDimension(const Dimension *newDim);
  virtual bool setDimension(const Rect *rect);
  virtual void setEmptyDimension(const Rect *dimByRect);
  virtual bool setPixelFormat(const PixelFormat *pixelFormat);
  virtual void setEmptyPixelFmt(const PixelFormat *pf);
  virtual bool setProperties(const Dimension *newDim, const PixelFormat *pixelFormat);
  virtual bool setProperties(const Rect *dimByRect, const PixelFormat *pixelFormat);
  virtual void setPropertiesWithoutResize(const Dimension *newDim, const PixelFormat *pf);
  virtual void setBuffer(void *newBuffer);

private:
  // This function updates a DIB section in accord with the FrameBuffer
  void *updateDibSection(const Dimension *newDim,
    const PixelFormat *pixelFormat,
    HWND compatibleWindow);
  void releaseDibSection();

  // This function generates an Exception if DIB section is not initialized yet.
  void checkDibValid();

  FrameBuffer m_fb;
  DibSection *m_dibSection;
};

#endif // __DIBFRAMEBUFFER_H__
