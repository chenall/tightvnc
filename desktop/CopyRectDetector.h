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

#ifndef __COPYRECTDETECTOR_H__
#define __COPYRECTDETECTOR_H__

#include "region/Rect.h"
#include "region/Point.h"
#include "util/CommonHeader.h"
#include <list>

struct WinProp
{
  WinProp(HWND _hwnd, Rect *_prevRect) { hwnd = _hwnd; prevRect = *_prevRect; }
  HWND hwnd;
  Rect prevRect;
};

class CopyRectDetector
{
public:
  CopyRectDetector();
  virtual ~CopyRectDetector();

  void detectWindowMovements(Rect *copyRect, Point *source);

protected:
  static BOOL CALLBACK enumWindowsFnCopyRect(HWND hwnd, LPARAM arg);
  BOOL checkWindowMovements(HWND hwnd);

  bool getWinRect(HWND hwnd, Rect *winRect);

  // If window properties successfully was found then function returns
  // true. Else this function returns false.
  bool findPrevWinProps(HWND hwnd, Rect *rect);

  Rect m_copyRect;
  Point m_source;

  std::list<WinProp> m_lastWinProps;
  std::list<WinProp> m_newWinProps;
};

#endif // __COPYRECTDETECTOR_H__
