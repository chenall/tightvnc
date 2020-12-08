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

#ifndef __MIRRORSCREENDRIVER_H__
#define __MIRRORSCREENDRIVER_H__

#include "MirrorDriverClient.h"
#include "win-system/RegistryKey.h"
#include "DisplayEsc.h"
#include "thread/GuiThread.h"
#include "win-system/WindowsEvent.h"
#include "UpdateDetector.h"

class MirrorScreenDriver : public UpdateDetector
{
public:
  MirrorScreenDriver(UpdateKeeper *updateKeeper,
                     UpdateListener *updateListener,
                     LocalMutex *fbLocalMutex,
                     LogWriter *log);
  virtual ~MirrorScreenDriver();

  // Starts screen update detection if it not started yet.
  virtual void executeDetection();

  // Stops screen update detection.
  virtual void terminateDetection();

  virtual Dimension getScreenDimension();
  virtual FrameBuffer *getScreenBuffer();
  virtual bool grab(const Rect *rect = 0);

  virtual bool getPropertiesChanged();
  virtual bool getScreenSizeChanged();

  virtual bool applyNewProperties();

private:
  void initFrameBuffer();

  void startUpdateSearching();

  virtual void execute();
  virtual void onTerminate();

  MirrorDriverClient *m_mirrorClient;
  unsigned long m_lastCounter;
  FrameBuffer m_frameBuffer;
  // TO THINK: One may use a self mutex here, because do not
  // use external objects here.
  LocalMutex *m_fbMutex;

  WindowsEvent m_updateTimeout;

  LogWriter *m_log;
};

#endif // __MIRRORSCREENDRIVER_H__
