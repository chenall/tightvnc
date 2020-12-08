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

#ifndef __WINDOWSMOUSEGRABBER_H__
#define __WINDOWSMOUSEGRABBER_H__

#include "CursorShapeGrabber.h"
#include "util/CommonHeader.h"
#include "win-system/Screen.h"

class WindowsCursorShapeGrabber : public CursorShapeGrabber
{
public:
  WindowsCursorShapeGrabber(void);
  virtual ~WindowsCursorShapeGrabber(void);

  virtual bool grab(const PixelFormat *pixelFormat);

  virtual bool isCursorShapeChanged();

private:
  bool grabPixels(const PixelFormat *pixelFormat);

  HCURSOR getHCursor();

  HCURSOR m_lastHCursor;
  Screen m_screen;
};

#endif // __WINDOWSMOUSEGRABBER_H__
