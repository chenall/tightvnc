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

#ifndef __VIEWPORTSTATE_H__
#define __VIEWPORTSTATE_H__

#include "region/Rect.h"

// This class stores a view port state.
class ViewPortState
{
public:
  ViewPortState();
  ViewPortState(const ViewPortState &);
  ViewPortState & operator =(const ViewPortState &viewPortState);
  ~ViewPortState();

  // After calling this function the update() function will set the view
  // port rectangle equal to it's the fbDimension argument rectangle.
  void setFullDesktop();

  // After calling this function the update() function will set the view
  // port rectangle to the *rect rectangle constrained by the fbDimension
  // update() function argument.
  void setArbitraryRect(const Rect *rect);

  // After calling this function the update() function will set the view
  // port rectangle by the primary display coordinates.
  void setPrimaryDisplay();

  // After calling this function the update() function will set the view
  // port rectangle by the display coordinates with a number.
  void setDisplayNumber(unsigned char displayNumber);

  // After calling this function the update() function will set the view
  // port rectangle by a window that own the hwnd argument.
  void setWindowHandle(HWND hwnd);

  // After calling this function the update() function will set the view
  // port rectangle by a window that own the windowName argument.
  void setWindowName(const StringStorage *windowName);

  // After calling this function the hwnd will be zeroid and state
  // will be changed to unresolved.
  void unresolveHwnd();

  // After calling this function the update() function will set the view
  // port rectangle to "Full desktop" and the view port region to a region constrained
  // by windows of the process id.
  void setProcessId(unsigned int processId);

private:
  static const int FULL_DESKTOP = 0;
  static const int PRIMARY_DISPLAY = 1;
  static const int ARBITRARY_RECT = 2;
  static const int WINDOW_RECT = 3;
  static const int DISPLAY_NUMBER = 4;
  static const int APPLICATION = 5;

  int m_mode;
  Rect m_arbitraryRect;
  HWND m_hwnd;
  StringStorage m_windowName;
  bool m_windowIsResolved;
  unsigned char m_displayNumber;
  unsigned int m_processId;

  friend class ViewPort;
};

#endif // __VIEWPORTSTATE_H__
