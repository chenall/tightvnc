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

#include "CursorPainter.h"

#include "thread/AutoLock.h"

CursorPainter::CursorPainter(FrameBuffer *fb, LogWriter *logWriter)
: m_fb(fb),
  m_logWriter(logWriter),
  m_cursorIsMoveable(false),
  m_ignoreShapeUpdates(false),
  m_isExist(false)
{
}

CursorPainter::~CursorPainter()
{
}

void CursorPainter::updatePointerPos(const Point *position)
{
  AutoLock al(&m_lock);
  m_pointerPosition = *position;
  m_cursorIsMoveable = true;

  // Now, cursor is ready for painting.
}

void CursorPainter::setNewCursor(const Point *hotSpot,
                                 UINT16 width, UINT16 height,
                                 const vector<UINT8> *cursor,
                                 const vector<UINT8> *bitmask)
{
  AutoLock al(&m_lock);
  m_logWriter->debug(_T("Cursor hot-spot is (%d, %d)"), hotSpot->x, hotSpot->y);
  m_cursor.setHotSpot(hotSpot->x, hotSpot->y);

  m_logWriter->debug(_T("Cursor size is (%d, %d)"), width, height);
  Dimension cursorDimension(width, height);
  PixelFormat pixelFormat = m_fb->getPixelFormat();

  m_cursor.setProperties(&cursorDimension, &pixelFormat);
  m_cursorOverlay.setProperties(&cursorDimension, &pixelFormat);

  size_t pixelSize = m_fb->getBytesPerPixel();
  size_t cursorSize = width * height * pixelSize;
  // Server is allowed to specify zero as width and/or height of the cursor.
  if (cursorSize != 0) {
    m_logWriter->debug(_T("Set image of cursor..."));
    memcpy(m_cursor.getPixels()->getBuffer(), &cursor->front(), cursorSize);
    m_logWriter->debug(_T("Set bitmask of cursor..."));
    m_cursor.assignMaskFromRfb(reinterpret_cast<const char *>(&bitmask->front()));
  }
}

void CursorPainter::setIgnoreShapeUpdates(bool ignore)
{
  m_logWriter->debug(_T("Set flag of ignor by cursor update is '%d'"), ignore);

  AutoLock al(&m_lock);
  m_ignoreShapeUpdates = ignore;
}

Rect CursorPainter::hideCursor()
{
  AutoLock al(&m_lock);

  if (!m_isExist) {
    return Rect();
  }

  m_isExist = false;

  Rect erase(m_cursorOverlay.getDimension().getRect());
  Point corner = getUpperLeftPoint(&m_lastPosition);

  erase.move(corner.x, corner.y);

  m_logWriter->info(_T("Cursor rect: (%d, %d), (%d, %d)"), erase.left, erase.top, erase.right, erase.bottom);

  if (erase.area() == 0) {
    return Rect();
  }

  m_logWriter->debug(_T("Erasing cursor..."));
  m_fb->copyFrom(&erase, &m_cursorOverlay, 0, 0);

  return erase;
}

Rect CursorPainter::showCursor()
{
  AutoLock al(&m_lock);

  m_lastPosition = m_pointerPosition;

  if (m_isExist) {
    m_logWriter->error(_T("Error in CursorPainter: painting double copy of cursor."));
    _ASSERT(true);
  }

  if (!m_ignoreShapeUpdates && m_cursorIsMoveable && m_cursor.getDimension().area() != 0) {

    m_logWriter->debug(_T("Painting cursor..."));

    Point corner = getUpperLeftPoint(&m_lastPosition);

    m_cursorOverlay.copyFrom(m_fb, corner.x, corner.y);

    Rect overlayRect(&m_cursor.getDimension().getRect());
    overlayRect.move(corner.x, corner.y);

    m_fb->overlay(&overlayRect, m_cursor.getPixels(), 0, 0, m_cursor.getMask());

    m_isExist = true;
    return overlayRect;
  }
  return Rect();
}

Point CursorPainter::getUpperLeftPoint(const Point *position) const
{
  Point upperLeftPoint = *position;
  upperLeftPoint.move(-m_cursor.getHotSpot().x, -m_cursor.getHotSpot().y);
  return upperLeftPoint;
}
