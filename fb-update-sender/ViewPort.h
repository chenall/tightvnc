// Copyright (C) 2009,2010,2011,2012 GlavSoft LLC.
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

#ifndef __VIEWPORT_H__
#define __VIEWPORT_H__

#include "ViewPortState.h"
#include "rfb/FrameBuffer.h"
#include "desktop/Desktop.h"
#include "thread/LocalMutex.h"
#include "util/DateTime.h"
#include "log-writer/LogWriter.h"

// This class calculates actual view port rectangle.
// Typical usage:
// // Initialisation
// ViewPort m_viewPort;
// ...
// ...
// m_viewPort.update(&frameBuffer->getDimension());
// Rect viewPort = m_viewPort.getViewPortRect();
class ViewPort
{
public:
  ViewPort(LogWriter *log);
  ViewPort(const ViewPortState *viewPortState, LogWriter *log);
  ~ViewPort();

  // Sets desktop interface that can be used in some mode to get
  // desktop info. The desktop interface uses only in the update() function.
  void initDesktopInterface(Desktop *desktop);

  // This function updates view port rectangle. The new view port rectangle
  // will be constrained by fbDimension.
  void update(const Dimension *fbDimension);

  // This function returns the view port rectangle.
  Rect getViewPortRect();

  // Returns true if checked share only application.
  bool getOnlyApplication();

  // Returns application PID if we share one application. Check getOnlyApplication() as well,
  // to make sure the returned value is relevant.
  unsigned int getApplicationId();

  // Returns a region that has rectangles of application which was visible
  // window parts at latest calling of the update() function. The region
  // doesn't changes by the update function if checked mode isn't "shareapp".
  void getApplicationRegion(Region *region);

  // Assignes self values by an external state.
  void changeState(const ViewPortState *newState);

private:
  // Disable the copy operation and constructor.
  ViewPort(const ViewPort &);
  ViewPort & operator =(const ViewPort &);

  // Resolves a window name of the view port state to window handle.
  // On an error the function do nothing.
  void resolveWindowName();

  static const int RESOLVING_PERIOD = 3000;

  Desktop *m_desktop;

  ViewPortState m_state;
  Rect m_rect;
  Region m_appRegion;
  LocalMutex m_stateMutex;

  DateTime m_latestHwndResolvingTime;

  LogWriter *m_log;
};

#endif // __VIEWPORT_H__
