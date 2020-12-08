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

#include "Win32ScreenDriverBaseImpl.h"
#include "util/Exception.h"

Win32ScreenDriverBaseImpl::Win32ScreenDriverBaseImpl(UpdateKeeper *updateKeeper,
                                                 UpdateListener *updateListener,
                                                 LocalMutex *fbLocalMutex,
                                                 LogWriter *log)
: WinVideoRegionUpdaterImpl(log),
  m_fbLocalMutex(fbLocalMutex),
  m_cursorPosDetector(updateKeeper, updateListener, log),
  m_curShapeDetector(updateKeeper, updateListener, &m_curShapeGrabber, fbLocalMutex, log)
{
}

Win32ScreenDriverBaseImpl::~Win32ScreenDriverBaseImpl()
{
  terminateDetection();
}

void Win32ScreenDriverBaseImpl::executeDetection()
{
  m_cursorPosDetector.resume();
  m_curShapeDetector.resume();
}

void Win32ScreenDriverBaseImpl::terminateDetection()
{
  m_cursorPosDetector.terminate();
  m_curShapeDetector.terminate();

  m_cursorPosDetector.wait();
  m_curShapeDetector.wait();
}

LocalMutex *Win32ScreenDriverBaseImpl::getFbMutex()
{
  return m_fbLocalMutex;
}

bool Win32ScreenDriverBaseImpl::grabCursorShape(const PixelFormat *pf)
{
  // Grabbing under the mutex avoid us from grab void cursor shape in time when the
  // shape hides until grabs screen.
  AutoLock al(m_fbLocalMutex);
  return m_curShapeGrabber.grab(pf);
}

const CursorShape *Win32ScreenDriverBaseImpl::getCursorShape()
{
  return m_curShapeGrabber.getCursorShape();
}

Point Win32ScreenDriverBaseImpl::getCursorPosition()
{
  AutoLock al(m_fbLocalMutex);
  return m_cursorPosDetector.getCursorPos();
}

void Win32ScreenDriverBaseImpl::getCopiedRegion(Rect *copyRect, Point *source)
{
  AutoLock al(m_fbLocalMutex);
  m_copyRectDetector.detectWindowMovements(copyRect, source);
}
