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

#ifndef __WIN32MIRRORSCREENDRIVER_H__
#define __WIN32MIRRORSCREENDRIVER_H__

#include "MirrorScreenDriver.h"
#include "Win32ScreenDriverBaseImpl.h"

class Win32MirrorScreenDriver : public Win32ScreenDriverBaseImpl
{
public:
  Win32MirrorScreenDriver(UpdateKeeper *updateKeeper,
                          UpdateListener *updateListener,
                          LocalMutex *fbLocalMutex,
                          LogWriter *log);
  virtual ~Win32MirrorScreenDriver();

  // Starts screen update detection if it not started yet.
  virtual void executeDetection();

  // Stops screen update detection.
  virtual void terminateDetection();

  // Note: This class is a wrapper for the MirrorScreenDriver class and then
  // only MirrorScreenDriver can provide appropriate thread safety for the ScreenDriver functions.

  virtual Dimension getScreenDimension();
  virtual bool grabFb(const Rect *rect = 0);
  virtual FrameBuffer *getScreenBuffer();
  virtual bool getScreenPropertiesChanged();
  virtual bool getScreenSizeChanged();
  virtual bool applyNewScreenProperties();

private:
  MirrorScreenDriver m_mirrorDriver;
};

#endif // __WIN32MIRRORSCREENDRIVER_H__
