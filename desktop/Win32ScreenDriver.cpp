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

#include "Win32ScreenDriver.h"
#include "thread/AutoLock.h"

Win32ScreenDriver::Win32ScreenDriver(UpdateKeeper *updateKeeper,
                                     UpdateListener *updateListener,
                                     FrameBuffer *fb,
                                     LocalMutex *fbLocalMutex, LogWriter *log)
: Win32ScreenDriverBaseImpl(updateKeeper, updateListener, fbLocalMutex, log),
  m_poller(updateKeeper, updateListener, &m_screenGrabber, fb, fbLocalMutex, log),
  m_consolePoller(updateKeeper, updateListener, &m_screenGrabber, fb, fbLocalMutex, log),
  m_hooks(updateKeeper, updateListener, log)
{
  // At this point the screen driver has valid screen properties (provides by screen grabber).
}

Win32ScreenDriver::~Win32ScreenDriver()
{
  terminateDetection();
}

void Win32ScreenDriver::executeDetection()
{
  Win32ScreenDriverBaseImpl::executeDetection();
  m_poller.resume();
  m_consolePoller.resume();
  m_hooks.resume();
}

void Win32ScreenDriver::terminateDetection()
{
  m_poller.terminate();
  m_consolePoller.terminate();
  m_hooks.terminate();

  Win32ScreenDriverBaseImpl::terminateDetection();

  m_poller.wait();
  m_consolePoller.wait();
  m_hooks.wait();
}

Dimension Win32ScreenDriver::getScreenDimension()
{
  AutoLock al(getFbMutex());
  return Dimension(&m_screenGrabber.getScreenRect());
}

bool Win32ScreenDriver::grabFb(const Rect *rect)
{
  AutoLock al(getFbMutex());
  return m_screenGrabber.grab(rect);
}

FrameBuffer *Win32ScreenDriver::getScreenBuffer()
{
  return m_screenGrabber.getScreenBuffer();
}

bool Win32ScreenDriver::getScreenPropertiesChanged()
{
  AutoLock al(getFbMutex());
  return m_screenGrabber.getPropertiesChanged();
}

bool Win32ScreenDriver::getScreenSizeChanged()
{
  AutoLock al(getFbMutex());
  return m_screenGrabber.getScreenSizeChanged();
}

bool Win32ScreenDriver::applyNewScreenProperties()
{
  AutoLock al(getFbMutex());
  return m_screenGrabber.applyNewProperties();
}
