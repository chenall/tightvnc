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

#ifndef __WIN32SCREENDRIVER_H__
#define __WIN32SCREENDRIVER_H__

#include "Win32ScreenDriverBaseImpl.h"
#include "Poller.h"
#include "ConsolePoller.h"
#include "HooksUpdateDetector.h"
#include "WindowsScreenGrabber.h"

class Win32ScreenDriver : public Win32ScreenDriverBaseImpl
{
public:
  Win32ScreenDriver(UpdateKeeper *updateKeeper,
                       UpdateListener *updateListener,
                       FrameBuffer *fb,
                       LocalMutex *fbLocalMutex, LogWriter *log);
  virtual ~Win32ScreenDriver();

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

private:
  // This class provides thread safed coordinations between the backup frame buffer and
  // the following objects.
  WindowsScreenGrabber m_screenGrabber;
  Poller m_poller;
  ConsolePoller m_consolePoller;
  HooksUpdateDetector m_hooks;
};

#endif // __WIN32SCREENDRIVER_H__
