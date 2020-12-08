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

#include "ConsolePoller.h"
#include "server-config-lib/Configurator.h"

ConsolePoller::ConsolePoller(UpdateKeeper *updateKeeper,
                             UpdateListener *updateListener,
                             ScreenGrabber *screenGrabber,
                             FrameBuffer *backupFrameBuffer,
                             LocalMutex *frameBufferMutex,
                             LogWriter *log)
: UpdateDetector(updateKeeper, updateListener),
  m_screenGrabber(screenGrabber),
  m_backupFrameBuffer(backupFrameBuffer),
  m_frameBufferMutex(frameBufferMutex),
  m_log(log)
{
  m_pollingRect.setRect(0, 0, 16, 16);
}

ConsolePoller::~ConsolePoller()
{
  terminate();
  wait();
}

void ConsolePoller::onTerminate()
{
  m_intervalWaiter.notify();
}

void ConsolePoller::execute()
{
  m_log->info(_T("console poller thread id = %d"), getThreadId());

  Rect scanRect;
  Region region;
  while (!isTerminating()) {
    Rect conRect = getConsoleRect();
    if (!conRect.isEmpty()) {
      int pollHeight = m_pollingRect.getHeight();
      int pollWidth = m_pollingRect.getWidth();

      {
        AutoLock al(m_frameBufferMutex);
        Rect offsetFb = m_screenGrabber->getScreenRect();
        conRect.move(-offsetFb.left, -offsetFb.top);
        FrameBuffer *screenFrameBuffer = m_screenGrabber->getScreenBuffer();
        if (screenFrameBuffer->isEqualTo(m_backupFrameBuffer)) {
          m_screenGrabber->grab(&conRect);
          for (int iRow = conRect.top; iRow < conRect.bottom; iRow += pollHeight) {
            for (int iCol = conRect.left; iCol < conRect.right; iCol += pollWidth) {
              scanRect.setRect(iCol, iRow, min(iCol + pollWidth, conRect.right),
                               min(iRow + pollHeight, conRect.bottom));
              if (!screenFrameBuffer->cmpFrom(&scanRect, m_backupFrameBuffer,
                                              scanRect.left, scanRect.top)) {
                region.addRect(&scanRect);
              }
            }
          }
        }
      }

      // Send event
      if (!region.isEmpty()) {
        m_updateKeeper->addChangedRegion(&region);
        doUpdate();
      }
    }
    unsigned int pollInterval = 200;
    m_intervalWaiter.waitForEvent(pollInterval);
  }
}

Rect ConsolePoller::getConsoleRect()
{
  Rect rect;
  HWND hwnd = GetForegroundWindow();

  const TCHAR consoleClassName[] = _T("ConsoleWindowClass");

  const size_t nameLength = sizeof(consoleClassName) / sizeof(TCHAR) + 1;
  TCHAR className[nameLength];
  GetClassName(hwnd, className, nameLength);
  if (_tcscmp(consoleClassName, className) == 0) {
    RECT winRect;
    GetWindowRect(hwnd, &winRect);
    rect.fromWindowsRect(&winRect);
  }
  return rect;
}
