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

#ifndef _FB_UDPATE_NOTIFIER_H_
#define _FB_UPDATE_NOTIFIER_H_

#include "log-writer/LogWriter.h"
#include "rfb/FrameBuffer.h"
#include "region/Point.h"
#include "region/Rect.h"
#include "region/Region.h"
#include "thread/LocalMutex.h"
#include "thread/Thread.h"
#include "win-system/WindowsEvent.h"

#include "CursorPainter.h"
#include "WatermarksController.h"

class CoreEventsAdapter;

class FbUpdateNotifier : public Thread
{
public:
  FbUpdateNotifier(FrameBuffer *fb, LocalMutex *fbLock, LogWriter *logger, WatermarksController* wmController);
  virtual ~FbUpdateNotifier();

  void setAdapter(CoreEventsAdapter *adapter);

  void onUpdate(const Rect *rect);
  void onPropertiesFb();

  void updatePointerPos(const Point *position);
  void setNewCursor(const Point *hotSpot,
                    UINT16 width, UINT16 height,
                    const vector<UINT8> *cursor, 
                    const vector<UINT8> *bitmask);

  void setIgnoreShapeUpdates(bool ignore);
protected:
  // Inherited from Thread
  void execute();
  void onTerminate();

  LocalMutex *m_fbLock;
  FrameBuffer *m_frameBuffer;
  CursorPainter m_cursorPainter;

  // Pointer to adapter.
  // Nothing event (changing properties of frame buffer, update frame buffer
  // or update cursor) don't sended to adapter, while m_adapter is 0.
  CoreEventsAdapter *m_adapter;

  LocalMutex m_updateLock;
  WindowsEvent m_eventUpdate;

  LogWriter *m_logWriter;

  //It is used for adding watermarks in demo version.
  WatermarksController* m_watermarksController;

  // In this region added all updates of frame buffer and cursor updates.
  Region m_update;

  // This rectangle save position of cursor.
  Rect m_oldPosition;

  // This flag is true after call onPropertiesFb().
  bool m_isNewSize;

  // This flag is true after set new cursor or update position.
  bool m_isCursorChange;

private:
  // Do not allow copying objects.
  FbUpdateNotifier(const FbUpdateNotifier &);
  FbUpdateNotifier &operator=(const FbUpdateNotifier &);
};

#endif
