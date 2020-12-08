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

#include "WindowsDisplays.h"
#include "thread/AutoLock.h"

WindowsDisplays::WindowsDisplays()
{
}

WindowsDisplays::~WindowsDisplays()
{
}

BOOL CALLBACK WindowsDisplays::monitorEnumProc(HMONITOR hMonitor,
                                               HDC hdcMonitor,
                                               LPRECT lprcMonitor,
                                               LPARAM dwData)
{
  WindowsDisplays *_this = (WindowsDisplays *)dwData;
  Rect rect(lprcMonitor->left - _this->m_xVirtualScreen,
            lprcMonitor->top - _this->m_yVirtualScreen,
            lprcMonitor->right - _this->m_xVirtualScreen,
            lprcMonitor->bottom - _this->m_yVirtualScreen);
  _this->m_displayRects.push_back(rect);
  return TRUE;
}

void WindowsDisplays::update()
{
  if (!isAlreadyUpdated()) {
    m_displayRects.clear();
    m_xVirtualScreen = GetSystemMetrics(SM_XVIRTUALSCREEN);
    m_yVirtualScreen = GetSystemMetrics(SM_YVIRTUALSCREEN);

    // Enumerate only desktop's displays. Skip mirror driver desktops.
    HDC hdc = GetDC(0);
    EnumDisplayMonitors(hdc, 0, monitorEnumProc, (LPARAM)this);

    m_latestUpdateTime = DateTime::now();
  }
}

void WindowsDisplays::getDisplayCoordinates(unsigned char displayNumber,
                                            Rect *rect)
{
  AutoLock al(&m_displayRectsMutex);
  update();
  displayNumber--;
  if (displayNumber < m_displayRects.size()) {
    *rect = m_displayRects[displayNumber];
  } else {
    rect->clear();
  }
}

bool WindowsDisplays::isAlreadyUpdated()
{
  if ((DateTime::now() - m_latestUpdateTime).getTime() > UPDATE_INTERVAL) {
    return false;
  } else {
    return true;
  }
}

std::vector<Rect> WindowsDisplays::getDisplays()
{
  update();
  return m_displayRects;
}
