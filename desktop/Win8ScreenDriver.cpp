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

#include "Win8ScreenDriver.h"
#include "util/Exception.h"

Win8ScreenDriver::Win8ScreenDriver(UpdateKeeper *updateKeeper,
                                   UpdateListener *updateListener,
                                   LocalMutex *fbLocalMutex,
                                   LogWriter *log)
: WinVideoRegionUpdaterImpl(log),
  m_log(log),
  m_fbLocalMutex(fbLocalMutex),
  m_updateKeeper(updateKeeper),
  m_updateListener(updateListener),
  m_detectionEnabled(false)
{
  AutoLock al(&m_drvImplMutex);
  m_drvImpl = new Win8ScreenDriverImpl(m_log, m_updateKeeper, m_fbLocalMutex, m_updateListener);
}

Win8ScreenDriver::~Win8ScreenDriver()
{
  terminateDetection();
  AutoLock al(&m_drvImplMutex);
  if (m_drvImpl != 0) {
    delete m_drvImpl;
    m_drvImpl = 0;
  }
}

void Win8ScreenDriver::executeDetection()
{
  AutoLock al(&m_drvImplMutex);
  m_detectionEnabled = true;
  m_drvImpl->executeDetection();
}

void Win8ScreenDriver::terminateDetection()
{
  AutoLock al(&m_drvImplMutex);
  m_detectionEnabled = false;
  if (m_drvImpl != 0) {
    m_drvImpl->terminateDetection();
  }
}

Dimension Win8ScreenDriver::getScreenDimension()
{
  AutoLock al(&m_drvImplMutex);
  return m_drvImpl->getScreenBuffer()->getDimension();
}

FrameBuffer *Win8ScreenDriver::getScreenBuffer()
{
  AutoLock al(&m_drvImplMutex);
  return m_drvImpl->getScreenBuffer();
}

bool Win8ScreenDriver::grabFb(const Rect *rect)
{
  AutoLock al(&m_drvImplMutex);
  return m_drvImpl->grabFb(rect);
}

bool Win8ScreenDriver::getScreenPropertiesChanged()
{
  AutoLock al(&m_drvImplMutex);
  return !m_drvImpl->isValid();
}

bool Win8ScreenDriver::getScreenSizeChanged()
{
  AutoLock al(&m_drvImplMutex);
  return !m_drvImpl->isValid();
}

bool Win8ScreenDriver::applyNewScreenProperties()
{
  try {
    m_log->debug(_T("Applying new screen properties, deleting old Win8ScreenDriverImpl"));
    AutoLock al(&m_drvImplMutex);
    if (m_drvImpl != 0) {
      delete m_drvImpl;
      m_drvImpl = 0;
    }
    m_log->debug(_T("Applying new screen properties, creating new Win8ScreenDriverImpl"));
    Win8ScreenDriverImpl *drvImpl =
      new Win8ScreenDriverImpl(m_log, m_updateKeeper, m_fbLocalMutex, m_updateListener, m_detectionEnabled);
    m_drvImpl = drvImpl;
  } catch (Exception &e) {
    m_log->error(_T("Can't apply new screen properties: %s"), e.getMessage());
    return false;
  }
  return true;
}

bool Win8ScreenDriver::grabCursorShape(const PixelFormat *pf)
{
  AutoLock al(&m_drvImplMutex);
  m_drvImpl->updateCursorShape(&m_cursorShape);
  return !m_drvImpl->isValid();
}

const CursorShape *Win8ScreenDriver::getCursorShape()
{
  return &m_cursorShape;
}

Point Win8ScreenDriver::getCursorPosition()
{
  AutoLock al(&m_drvImplMutex);
  return m_drvImpl->getCursorPosition();
}

void Win8ScreenDriver::getCopiedRegion(Rect *copyRect, Point *source)
{
  AutoLock al(m_fbLocalMutex);
  m_copyRectDetector.detectWindowMovements(copyRect, source);
}
