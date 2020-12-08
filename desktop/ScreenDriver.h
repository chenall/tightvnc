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

#ifndef __SCREENDRIVER_H__
#define __SCREENDRIVER_H__

#include "region/Dimension.h"
#include "region/Region.h"
#include "rfb/FrameBuffer.h"
#include "rfb/CursorShape.h"

// Derived classes commits oneself that internal frame buffer (accessing with getScreenBuffer()) will
// be valid after constructor until the applyNewScreenProperties() function will be called. If
// the applyNewScreenProperties() function has been called, internal frame buffer will be changed
// with the frame buffer invalidation and a new getScreenBuffer() call is required to get a valid
// frame buffer.
class ScreenDriver
{
public:
  // Derived constructors will ensure that the getScreenBuffer() returns
  // a frame buffer with valid screen properties.

  virtual ~ScreenDriver() {}

  // Starts screen update detection if it not started yet.
  virtual void executeDetection() = 0;

  // Stops screen update detection.
  virtual void terminateDetection() = 0;

  // Return a current screen Dimension.
  // Implementions will ensure that this function is thread safety.
  virtual Dimension getScreenDimension() = 0;

  // Provides FrameBuffer grabbing.
  // Parameters:     *rect - Pointer to a Rect object with relative workRect coordinates.
  // Return value:   true if success.
  // Implementions will ensure that this function is thread safety.
  virtual bool grabFb(const Rect *rect = 0) = 0;

  // Returns a pointer an internal screen driver FrameBuffer
  virtual FrameBuffer *getScreenBuffer() = 0;

  // Checks screen(desktop) properties on changes
  // Implementions will ensure that this function is thread safety.
  virtual bool getScreenPropertiesChanged() = 0;

  // Implementions will ensure that this function is thread safety.
  virtual bool getScreenSizeChanged() = 0;

  // Set new values of the WorkRect to default (to full screen rectangle coordinates)
  // if desktop properties has been changed.
  // Also the frame buffer pixel format set to actual value.
  // Implementions will ensure that this function is thread safety.
  virtual bool applyNewScreenProperties() = 0;

  // Grabs current cursor shape with the pixel format to internal buffer.
  // Implementions will ensure that this function is thread safety.
  virtual bool grabCursorShape(const PixelFormat *pf) = 0;

  // Returns a pointer an internal screen driver CursorShape
  virtual const CursorShape *getCursorShape() = 0;

  // Returns current cursor position coordinates which is relative to frame buffer coordinates.
  // Implementions will ensure that this function is thread safety.
  virtual Point getCursorPosition() = 0;

  // Returns a region as known "copy region".
  // Implementions will ensure that this function is thread safety.
  virtual void getCopiedRegion(Rect *copyRect, Point *source) = 0;

  // Returns a region which associates with some video data. The region will define
  // by concrete implementation.
  // Implementions will not ensure that this function is thread safety.
  virtual Region getVideoRegion() = 0;
};

#endif // __SCREENDRIVER_H__
