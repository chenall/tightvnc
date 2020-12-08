// Copyright (C) 2012 GlavSoft LLC.
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

#ifndef __SYSTEM_INFORMATION_H__
#define __SYSTEM_INFORMATION_H__

#include "util/CommonHeader.h"

class SystemInformation {
public:

  // get rectangle of all desktop area
  void getDesktopAllArea(RECT * rc);

  // get rectangle of user desktop area
  // without button 'Start' (only primary monitor).
  bool getDesktopArea(RECT *rc);

  // return true if StretchBlt is used
  // in provided device
  bool isSupportStretchBlt(HDC hdc);

  // return how many bits per pixel
  // for provided device
  int getBitsPixel(HDC hdc);

  // get the number of monitors that plugged 
  // into video card
  int getMonitorCount();
};

#endif
