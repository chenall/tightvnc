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

#ifndef _SCROLL_BAR_H_
#define _SCROLL_BAR_H_

#include "util/CommonHeader.h"

// ScrollBar class
// This class doesn't create any window, it only show/hide that is already
// contained in existed window.

class ScrollBar
{
public:
  ScrollBar();

  void setWindow(HWND hwnd);
  bool showVertScroll(bool show);
  bool showHorzScroll(bool show);
  void setHorzRange(int imin, int imax, int istep);
  void setVertRange(int imin, int imax, int istep);
  void setVertPos(int iPos);
  void setHorzPos(int iPos);
  void moveUpVert(int iPercent = 0);
  void moveDownVert(int iPercent = 0);
  void moveLeftHorz(int iPercent = 0);
  void moveRightHorz(int inPercent = 0);
  int getVertPos();
  int getHorzPos();
  int getVerticalSize();
  int getHorizontalSize();

  static const int SCROLL_STEP = 5;
protected:
  void enableVirtualScroll();
  void disableVirtualScroll();
  bool _showVertScroll(bool show);
  bool _showHorzScroll(bool show);

  HWND m_hwnd;
  int m_vPos, m_hPos;
  int m_vMin, m_vMax;
  int m_hMin, m_hMax;
  int m_vStep;
  int m_hStep;
  bool m_isVirtualScroll;
  bool m_isVert;
  bool m_isHorz;
  bool m_isVVert;
  bool m_isVHorz;
};

#endif
