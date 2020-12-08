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

#ifndef __DUMMYSCREENDRIVER_H__
#define __DUMMYSCREENDRIVER_H__
#include "ScreenDriver.h"
#include "log-writer/LogWriter.h"
#include "UpdateKeeper.h"
#include "UpdateListener.h"
#include "thread/Thread.h"
#include "win-system/WindowsEvent.h"


class DummyScreenDriver : public ScreenDriver, Thread
{
public:
  DummyScreenDriver(UpdateKeeper *updateKeeper, UpdateListener *updateListener, Dimension dim, unsigned int interval, LogWriter *log);
  virtual ~DummyScreenDriver();

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
  bool grabCursorShape(const PixelFormat *pf) { return true; };
  const CursorShape *getCursorShape() { return &m_cursorShape; };
  Point getCursorPosition() { return Point(); };

  void getCopiedRegion(Rect *copyRect, Point *source) { return; };
  Region getVideoRegion() { return Region(); };

protected:
  virtual void execute();
  virtual void onTerminate();

private:
  FrameBuffer m_workFrameBuffer;
  CursorShape m_cursorShape;
  UpdateKeeper * m_updateKeeper;
  UpdateListener * m_updateListener;
  WindowsEvent m_sleeper;
  unsigned int m_interval;
  bool m_detectionEnabled;

};

#endif // __DUMMYSCREENDRIVER_H__
