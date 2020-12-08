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

#include "ViewPortState.h"

ViewPortState::ViewPortState()
: m_mode(FULL_DESKTOP),
  m_hwnd(0),
  m_windowIsResolved(false),
  m_displayNumber(1),
  m_processId(0)
{
}

ViewPortState::ViewPortState(const ViewPortState &viewPortState)
{
  *this = viewPortState;
}

ViewPortState & ViewPortState::operator =(const ViewPortState &viewPortState)
{
  m_mode = viewPortState.m_mode;
  m_arbitraryRect = viewPortState.m_arbitraryRect;
  m_hwnd = viewPortState.m_hwnd;
  m_windowName = viewPortState.m_windowName;
  m_windowIsResolved = viewPortState.m_windowIsResolved;
  m_displayNumber = viewPortState.m_displayNumber;
  m_processId = viewPortState.m_processId;

  return *this;
}

ViewPortState::~ViewPortState()
{
}

void ViewPortState::setFullDesktop()
{
  m_mode = FULL_DESKTOP;
}

void ViewPortState::setArbitraryRect(const Rect *rect)
{
  m_mode = ARBITRARY_RECT;
  m_arbitraryRect = *rect;
}

void ViewPortState::setPrimaryDisplay()
{
  m_mode = PRIMARY_DISPLAY;
}

void ViewPortState::setDisplayNumber(unsigned char displayNumber)
{
  m_mode = DISPLAY_NUMBER;
  m_displayNumber = displayNumber;
}

void ViewPortState::setWindowHandle(HWND hwnd)
{
  m_mode = WINDOW_RECT;
  m_hwnd = hwnd;
  m_windowIsResolved = true;
}

void ViewPortState::setWindowName(const StringStorage *windowName)
{
  m_mode = WINDOW_RECT;
  m_windowName = *windowName;
  m_windowIsResolved = false;
}

void ViewPortState::unresolveHwnd()
{
  m_windowIsResolved = false;
  m_hwnd = 0;
}

void ViewPortState::setProcessId(unsigned int processId)
{
  m_mode = APPLICATION;
  m_processId = processId;
}
