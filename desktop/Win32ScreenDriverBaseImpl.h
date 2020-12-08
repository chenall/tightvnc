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

#ifndef __WIN32SCREENDRIVERBASEIMPL_H__
#define __WIN32SCREENDRIVERBASEIMPL_H__

#include "ScreenDriver.h"
#include "WinVideoRegionUpdaterImpl.h"
#include "CursorPositionDetector.h"
#include "CursorShapeDetector.h"
#include "WindowsCursorShapeGrabber.h"
#include "CopyRectDetector.h"

// This class implements "grabbers" and "detectors" which is not coupleâ with screen frame buffer.
class Win32ScreenDriverBaseImpl : public WinVideoRegionUpdaterImpl
{
public:
  Win32ScreenDriverBaseImpl(UpdateKeeper *updateKeeper,
                          UpdateListener *updateListener,
                          LocalMutex *fbLocalMutex,
                          LogWriter *log);
  virtual ~Win32ScreenDriverBaseImpl();

  // Starts screen update detection if it not started yet.
  virtual void executeDetection();

  // Stops screen update detection.
  virtual void terminateDetection();

  virtual bool grabCursorShape(const PixelFormat *pf);
  virtual const CursorShape *getCursorShape();
  virtual Point getCursorPosition();

  virtual void getCopiedRegion(Rect *copyRect, Point *source);

protected:
  LocalMutex *getFbMutex();

private:
  LocalMutex *m_fbLocalMutex;

  CursorPositionDetector m_cursorPosDetector;
  WindowsCursorShapeGrabber m_curShapeGrabber;
  CursorShapeDetector m_curShapeDetector;

  CopyRectDetector m_copyRectDetector;
};

#endif // __WIN32SCREENDRIVERBASEIMPL_H__
