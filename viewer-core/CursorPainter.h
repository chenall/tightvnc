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

#ifndef _CURSOR_PAINTER_H_
#define _CURSOR_PAINTER_H_

#include "log-writer/LogWriter.h"
#include "rfb/CursorShape.h"
#include "thread/LocalMutex.h"

class CursorPainter
{
public:
  CursorPainter(FrameBuffer *fb, LogWriter *logWriter);
  virtual ~CursorPainter();

  // this functions is thread-safe for private data of cursor, but need external lock of frame buffer
  Rect hideCursor();
  Rect showCursor();

  // this functions is thread-safe
  void setIgnoreShapeUpdates(bool ignore);
  void updatePointerPos(const Point *position);
  void setNewCursor(const Point *hotSpot,
                    UINT16 width, UINT16 height,
                    const vector<UINT8> *cursor, 
                    const vector<UINT8> *bitmask);
private:
  // This function is thread-save.
  Point getUpperLeftPoint(const Point *position) const;

  LogWriter *m_logWriter;

  FrameBuffer *const m_fb;

  LocalMutex m_lock;
  CursorShape m_cursor;

  // Actual position of pointer
  Point m_pointerPosition;

  // Last painted position of pointer
  Point m_lastPosition;
  // Copy of rect frame buffer under cursor
  FrameBuffer m_cursorOverlay;

  // Flag is set, if cursor is showed.
  bool m_isExist;

  // Flag is set after first call updatePointerPosition().
  // If flag is unset then pointer isn't painted.
  bool m_cursorIsMoveable;

  bool m_ignoreShapeUpdates;

private:
  // Do not allow copying objects.
  CursorPainter(const CursorPainter &);
  CursorPainter &operator=(const CursorPainter &);
};

#endif
