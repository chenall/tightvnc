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

#ifndef __WINDOWSDISPLAYS_H__
#define __WINDOWSDISPLAYS_H__

#include "region/Rect.h"
#include "util/DateTime.h"
#include <vector>
#include "thread/LocalMutex.h"

class WindowsDisplays
{
public:
  WindowsDisplays();
  virtual ~WindowsDisplays();

  // If a display does not exist now the function return an empty rectangle.
  void getDisplayCoordinates(unsigned char displayNumber, Rect *rect);

  // Returns a vector that contain dispalys coordinates at the current time.
  std::vector<Rect> getDisplays();

private:
  // Updates internal information to a current state.
  void update();

  static BOOL CALLBACK monitorEnumProc(HMONITOR hMonitor,
                                       HDC hdcMonitor,
                                       LPRECT lprcMonitor,
                                       LPARAM dwData);

  // Returns true if the update() function has been called lately.
  bool isAlreadyUpdated();

  int m_xVirtualScreen;
  int m_yVirtualScreen;

  std::vector<Rect> m_displayRects;
  LocalMutex m_displayRectsMutex;
  
  static const unsigned int UPDATE_INTERVAL = 3000;
  DateTime m_latestUpdateTime;
};

#endif // __WINDOWSDISPLAYS_H__
