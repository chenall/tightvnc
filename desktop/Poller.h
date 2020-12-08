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

#ifndef __POLLER_H__
#define __POLLER_H__

#include "UpdateDetector.h"
#include "ScreenGrabber.h"
#include "rfb/FrameBuffer.h"
#include "region/Rect.h"
#include "win-system/WindowsEvent.h"
#include "log-writer/LogWriter.h"

#define DEFAULT_SLEEP_TIME 1000

class Poller : public UpdateDetector
{
public:
  Poller(UpdateKeeper *updateKeeper,
         UpdateListener *updateListener,
         ScreenGrabber *screenGrabber,
         FrameBuffer *backupFrameBuffer,
         LocalMutex *frameBufferCriticalSection,
         LogWriter *log);

  virtual ~Poller();

protected:
  virtual void execute();
  virtual void onTerminate();

private:
  ScreenGrabber *m_screenGrabber;
  FrameBuffer *m_backupFrameBuffer;
  LocalMutex *m_fbMutex;
  Rect m_pollingRect;
  WindowsEvent m_intervalWaiter;

  LogWriter *m_log;
};

#endif // __POLLER_H__
