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

#include "util/Exception.h"
#include "rfb/StandardPixelFormatFactory.h"
#include "WinDxCriticalException.h"
#include "WinDxRecoverableException.h"
#include <crtdbg.h>
#include "win-system/Screen.h"

#include "WinDxgiOutput.h"
#include "Win8DeskDuplicationThread.h"

// The header including of this cpp file must be at last place to avoid build conflicts.
#include "Win8ScreenDriverImpl.h"

Win8ScreenDriverImpl::Win8ScreenDriverImpl(LogWriter *log, UpdateKeeper *updateKeeper,
                                           LocalMutex *fbLocalMutex,
                                           UpdateListener *updateListener,
                                           bool detectionEnabled)
: m_updateKeeper(updateKeeper),
  m_updateListener(updateListener),
  m_log(log),
  m_curTimeStamp(0),
  m_hasCriticalError(false),
  m_hasRecoverableError(false),
  m_detectionEnabled(detectionEnabled)
{
  resume();
  // Wait for DxInterface initialization
  m_initEvent.waitForEvent();

  if (m_hasCriticalError) {
    terminate();
    wait();
    throw Exception(_T("Win8ScreenDriverImpl can't be successfully initialized"));
  }

  // Checking that builded dimension is equal to virtual desktop dimension.
  Dimension buildedDim = getScreenBuffer()->getDimension();
  Screen screen;
  Dimension virtDimension = screen.getDesktopDimension();
  if (!buildedDim.isEqualTo(&virtDimension)) {
    terminate();
    wait();
    throw Exception(_T("The builded screen dimension doesn't match to virtual screen dimension"));
  }

  // At this point the screen driver has valid screen properties.
}

Win8ScreenDriverImpl::~Win8ScreenDriverImpl()
{
  terminateDetection();
  terminate();
  int activeResult = (int)isActive();
  int waitResult = (int)wait();
  m_log->debug(_T("Win8ScreenDriverImpl::activeResult = %d"), activeResult);
  m_log->debug(_T("Win8ScreenDriverImpl::waitResult = %d"), waitResult);
}

void Win8ScreenDriverImpl::executeDetection()
{
  // Detection is already executed in the Win8 model, instead of we should enable it
  m_detectionEnabled = true;
}

void Win8ScreenDriverImpl::terminateDetection()
{
  m_log->debug(_T("Destroy Win8DeskDuplicationThreads"));
  m_deskDuplThreadBundle.destroyAllThreads();
  m_detectionEnabled = false;
}

FrameBuffer *Win8ScreenDriverImpl::getScreenBuffer()
{
  return &m_frameBuffer;
}

void Win8ScreenDriverImpl::initDxgi()
{
  m_log->debug(_T("Creating of D3D11Device"));
  WinD3D11Device d3D11Device(m_log);
  m_log->debug(_T("Quering Interface for IDXGIDevice"));
  WinDxgiDevice dxgiDevice(&d3D11Device);
  m_log->debug(_T("Getting Parent for IDXGIAdapter"));
  WinDxgiAdapter dxgiAdapter(&dxgiDevice);

  Region virtDeskRegion;
  m_log->debug(_T("Try to enumerate dxgi outputs"));
  std::vector<WinDxgiOutput> dxgiOutputArray;
  std::vector<Rect> deskCoordArray;
  UINT iOutput = 0;
  try {
    for (iOutput = 0; iOutput < 65535; iOutput++) {
      WinDxgiOutput dxgiOutput(&dxgiAdapter, iOutput);
      if (dxgiOutput.isAttachedtoDesktop()) {
        dxgiOutputArray.push_back(dxgiOutput);
        Rect deskCoord = dxgiOutput.getDesktopCoordinates();
        deskCoordArray.push_back(deskCoord);
        virtDeskRegion.addRect(&deskCoord);
      }
    }
  } catch (WinDxRecoverableException &) {
    m_log->debug(_T("Reached the end of dxgi output list with iOutput = %u"), iOutput);
    // End of output list.
  }
  m_log->debug(_T("We have %d dxgi output(s) connected"), dxgiOutputArray.size());

  // Check that all outputs for the virtual screen are found (in case two or more
  // hardware graphic interfaces are used). It's better to avoid using buggy
  // Desktop Duplication API here rather than getting the wrong framebuffer.
  Screen screen;
  if (screen.getVisibleMonitorCount() != dxgiOutputArray.size()) {
    throw Exception(_T("Unable get all DXGI outputs for virtual screen"));
  }

  PixelFormat pf = getDxPixelFormat();
  Rect virtDeskBoundRect = virtDeskRegion.getBounds();
  m_frameBuffer.setProperties(&virtDeskBoundRect, &pf);
  m_frameBuffer.setColor(0, 0, 0);

  for (size_t iDxgiOutput  = 0; iDxgiOutput < dxgiOutputArray.size(); iDxgiOutput++) {
    deskCoordArray[iDxgiOutput].move(-virtDeskBoundRect.left, -virtDeskBoundRect.top);
  }
  Thread *thread = new Win8DeskDuplicationThread(&m_frameBuffer,
    deskCoordArray,
    &m_win8CursorShape,
    &m_curTimeStamp,
    &m_cursorMutex,
    this,
    dxgiOutputArray,
    m_log);
  DWORD id = thread->getThreadId();
  m_log->debug(_T("Created a new Win8DeskDuplicationThread with ID: (%d)"), id);
  m_deskDuplThreadBundle.addThread(thread);
}

void Win8ScreenDriverImpl::execute()
{
  try {
    initDxgi();
    m_initEvent.notify();
  } catch (WinDxRecoverableException &e) {
    m_log->error(_T("Catched WinDxRecoverableException: %s, (%x)"), e.getMessage(), (int)e.getErrorCode());
    m_hasRecoverableError = true;
  } catch (WinDxCriticalException &e) {

    m_log->error(_T("Catched WinDxCriticalException: %s, (%x)"), e.getMessage(), (int)e.getErrorCode());
    m_hasCriticalError = true;
  } catch (Exception &e) {
    m_log->error(_T("Catched Exception in the Win8ScreenDriverImpl::execute() function: %s.")
                 _T(" The exception will consider as critical") , e.getMessage());
    m_hasCriticalError = true;
  }

  m_initEvent.notify();

  while (!isTerminating() && isValid()) {
    m_errorEvent.waitForEvent();
  }
  if (!isValid()) {
    m_log->error(_T("Win8ScreenDriverImpl has an invalid state. The invalid state can be")
                 _T(" a part of screen propery changes. An update signal will be generated")
                 _T(" as a screen size changed signal."));
    m_updateKeeper->setScreenSizeChanged();
    m_updateListener->onUpdate();
  }
  terminateDetection();
}

void Win8ScreenDriverImpl::onTerminate()
{
  m_errorEvent.notify();
}

void Win8ScreenDriverImpl::onFrameBufferUpdate(const Region *changedRegion)
{
  if (m_detectionEnabled) {
    m_updateKeeper->addChangedRegion(changedRegion);
    m_updateListener->onUpdate();
  }
}

void Win8ScreenDriverImpl::onCopyRect(const Rect *dstRect, int srcX, int srcY)
{
  if (m_detectionEnabled) {
    Point srcPoint(srcX, srcY);
    m_updateKeeper->addCopyRect(dstRect, &srcPoint);
    m_updateListener->onUpdate();
  }
}

void Win8ScreenDriverImpl::onCursorPositionChanged(int x, int y)
{
  AutoLock al(&m_cursorMutex);
  Point newPos(x, y);
  if (!m_latestCursorPos.isEqualTo(&newPos)) {
    m_latestCursorPos = newPos;
    m_updateKeeper->setCursorPosChanged(&newPos);
    m_updateListener->onUpdate();
  }
}

void Win8ScreenDriverImpl::onCursorShapeChanged()
{
  m_updateKeeper->setCursorShapeChanged();
  m_updateListener->onUpdate();
}

void Win8ScreenDriverImpl::onRecoverableError(const TCHAR *reason)
{
  m_log->error(_T("Win8ScreenDriverImpl catch an recoverable error with reason: %s"), reason);
  m_hasRecoverableError = true;
  m_errorEvent.notify();
}

void Win8ScreenDriverImpl::onCriticalError(const TCHAR *reason)
{
  m_log->error(_T("Win8ScreenDriverImpl catch an critical error with reason: %s"), reason);
  m_hasCriticalError = true;
  m_errorEvent.notify();
}

bool Win8ScreenDriverImpl::grabFb(const Rect *rect)
{
  return isValid();
}

bool Win8ScreenDriverImpl::isValid()
{
  return !m_hasRecoverableError && !m_hasCriticalError;
}

PixelFormat Win8ScreenDriverImpl::getDxPixelFormat() const
{
  return StandardPixelFormatFactory::create32bppPixelFormat();
}

void Win8ScreenDriverImpl::updateCursorShape(CursorShape *dst)
{
  AutoLock al(&m_cursorMutex);
  dst->clone(m_win8CursorShape.getCursorShape());
}

Point Win8ScreenDriverImpl::getCursorPosition()
{
  AutoLock al(&m_cursorMutex);
  return m_latestCursorPos;
}
