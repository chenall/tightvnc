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

#ifndef __WIN8SCREENDRIVER_H__
#define __WIN8SCREENDRIVER_H__

// FIXME: Remove this detectors when will be ready win8 detectors.
#include "CursorPositionDetector.h"
#include "CursorShapeDetector.h"
#include "WindowsCursorShapeGrabber.h"

#include "ScreenDriver.h"
#include "WinVideoRegionUpdaterImpl.h"
#include "UpdateKeeper.h"
#include "UpdateListener.h"
#include "log-writer/LogWriter.h"
#include "Win8ScreenDriverImpl.h"

class Win8ScreenDriver : public WinVideoRegionUpdaterImpl
{
public:
  // (Note: This class has no link to an external backup frame buffer and then it does not have
  // to use an external belonged mutex. Thread safe coordiantion commitments between this class
  // and external some elements should be entirely provided by an owner code.)

  Win8ScreenDriver(UpdateKeeper *updateKeeper,
                   UpdateListener *updateListener,
                   LocalMutex *fbLocalMutex,
                   LogWriter *log);
  virtual ~Win8ScreenDriver();

  // Starts screen update detection if it not started yet.
  virtual void executeDetection();

  // Stops screen update detection.
  virtual void terminateDetection();

  virtual Dimension getScreenDimension();
  virtual bool grabFb(const Rect *rect = 0);
  virtual FrameBuffer *getScreenBuffer();
  virtual bool getScreenPropertiesChanged();
  virtual bool getScreenSizeChanged();
  virtual bool applyNewScreenProperties();

  virtual bool grabCursorShape(const PixelFormat *pf);
  virtual const CursorShape *getCursorShape();
  virtual Point getCursorPosition();

  virtual void getCopiedRegion(Rect *copyRect, Point *source);

private:
  LogWriter *m_log;
  LocalMutex *m_fbLocalMutex;
  UpdateKeeper *m_updateKeeper;
  UpdateListener *m_updateListener;
  // This member must be always gueranted non zero. Otherwise an excption must
  // be provided from the constructor of this class.
  Win8ScreenDriverImpl *m_drvImpl;

  CursorShape m_cursorShape;

  bool m_detectionEnabled;
};

#endif // __WIN8SCREENDRIVER_H__
