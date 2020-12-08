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

#include "MirrorScreenDriver.h"
#include "util/Exception.h"

MirrorScreenDriver::MirrorScreenDriver(UpdateKeeper *updateKeeper,
                                       UpdateListener *updateListener,
                                       LocalMutex *fbLocalMutex,
                                       LogWriter *log)
: UpdateDetector(updateKeeper,
                 updateListener),
  m_fbMutex(fbLocalMutex),
  m_lastCounter(0),
  m_log(log)
{
  m_mirrorClient = new MirrorDriverClient(m_log);
  initFrameBuffer();
}

MirrorScreenDriver::~MirrorScreenDriver()
{
  terminateDetection();
  if (m_mirrorClient) {
    delete m_mirrorClient;
  }
}

void MirrorScreenDriver::executeDetection()
{
  resume();
}

void MirrorScreenDriver::terminateDetection()
{
  terminate();
  wait();
}

void MirrorScreenDriver::initFrameBuffer()
{
  Dimension dim = m_mirrorClient->getDimension();
  PixelFormat pf = m_mirrorClient->getPixelFormat();

  m_frameBuffer.setProperties(&dim, &pf);
}

Dimension MirrorScreenDriver::getScreenDimension()
{
  return m_frameBuffer.getDimension();
}

FrameBuffer *MirrorScreenDriver::getScreenBuffer()
{
  return &m_frameBuffer;
}

bool MirrorScreenDriver::grab(const Rect *rect)
{
  AutoLock al(m_fbMutex);

  if (m_mirrorClient == 0) {
    throw Exception(_T("Mirror driver client didn't initilized."));
  }

  Rect fbRect = m_frameBuffer.getDimension().getRect();
  Rect croppedRect;
  if (rect != 0) {
    croppedRect = rect->intersection(&fbRect);
  } else {
    croppedRect = fbRect;
  }

  char *dstPtr = (char *)m_frameBuffer.getBufferPtr(croppedRect.left,
                                                    croppedRect.top);
  size_t offset = dstPtr - (char *)m_frameBuffer.getBuffer();
  char *srcPtr = (char *)m_mirrorClient->getBuffer();
  if (srcPtr == 0) {
    return false;
  }
  srcPtr += offset;

  size_t count = croppedRect.getWidth() * m_frameBuffer.getBytesPerPixel();
  size_t stride = m_frameBuffer.getBytesPerRow();

  for (size_t i = croppedRect.top; i < croppedRect.bottom; i++,
                                                           dstPtr += stride,
                                                           srcPtr += stride) {
    memcpy(dstPtr, srcPtr, count);
  }
  return true;
}

bool MirrorScreenDriver::getPropertiesChanged()
{
  AutoLock al(m_fbMutex);
  if (m_mirrorClient != 0) {
    return m_mirrorClient->getPropertiesChanged();
  } else {
    throw Exception(_T("Mirror driver client didn't initilized."));
  }
}

bool MirrorScreenDriver::getScreenSizeChanged()
{
  AutoLock al(m_fbMutex);
  if (m_mirrorClient != 0) {
    return m_mirrorClient->getScreenSizeChanged();
  } else {
    throw Exception(_T("Mirror driver client didn't initilized."));
  }
}

bool MirrorScreenDriver::applyNewProperties()
{
  AutoLock al(m_fbMutex);

  delete m_mirrorClient;
  m_mirrorClient = 0;
  m_mirrorClient = new MirrorDriverClient(m_log);

  Dimension newDim = m_mirrorClient->getDimension();
  PixelFormat pf = m_mirrorClient->getPixelFormat();
  m_frameBuffer.setProperties(&newDim, &pf);
  m_lastCounter = 0;

  return true;
}

void MirrorScreenDriver::startUpdateSearching()
{
  resume();
}

void MirrorScreenDriver::execute()
{
  Region changedRegion;
  Rect changedRect;
  unsigned long currentCounter = 0;

  while (!isTerminating()) {
    m_updateTimeout.waitForEvent(20);

    {
      AutoLock al(m_fbMutex);
      if (m_mirrorClient != 0) {
        CHANGES_BUF *changesBuf = m_mirrorClient->getChangesBuf();
        if (changesBuf != 0) {
          currentCounter = changesBuf->counter;
          for (unsigned long i = m_lastCounter; i != currentCounter;
               i++, i%= MAXCHANGES_BUF) {
            changedRect.fromWindowsRect(&changesBuf->pointrect[i].rect);
            if (changedRect.isValid()) {
              changedRegion.addRect(&changedRect);
            }
          }

          m_updateKeeper->addChangedRegion(&changedRegion);
          m_lastCounter = currentCounter;
        }
      }
    }

    if (!changedRegion.isEmpty()) {
      doUpdate();
      changedRegion.clear();
    }
  }
}

void MirrorScreenDriver::onTerminate()
{
  m_updateTimeout.notify();
}
