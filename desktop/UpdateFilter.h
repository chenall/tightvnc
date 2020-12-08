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

#ifndef __UPDATEFILTER_H__
#define __UPDATEFILTER_H__

#include "WindowsScreenGrabber.h"
#include "ScreenDriver.h"
#include "rfb/FrameBuffer.h"
#include "thread/LocalMutex.h"
#include "UpdateContainer.h"
#include "GrabOptimizator.h"

class UpdateFilter
{
public:
  UpdateFilter(ScreenDriver *screenDriver,
               FrameBuffer *frameBuffer,
               LocalMutex *frameBufferCriticalSection,
               LogWriter *log);
  ~UpdateFilter();

  void filter(UpdateContainer *updateContainer);

private:
  void getChangedRegion(Region *rgn, const Rect *rect);
  void updateChangedRect(Region *rgn, const Rect *rect);
  void updateChangedSubRect(Region *rgn, const Rect *rect);

  // This function update the screen grabber frame buffer.
  // If success the function returns the true.
  // Also, this function researching an optimal way to grab from
  // the whole screen grabbing or 
  bool grab();

  ScreenDriver *m_screenDriver;
  FrameBuffer *m_frameBuffer;
  LocalMutex *m_fbMutex;
  GrabOptimizator m_grabOptimizator;

  LogWriter *m_log;
};

#endif // __UPDATEFILTER_H__
