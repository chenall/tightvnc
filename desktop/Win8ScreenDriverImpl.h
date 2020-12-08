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

#ifndef __WIN8SCREENDRIVERIMPL_H__
#define __WIN8SCREENDRIVERIMPL_H__

#include "log-writer/LogWriter.h"
#include "thread/GuiThread.h"
#include "thread/ThreadCollector.h"
#include "win-system/WindowsEvent.h"
#include "region/Rect.h"
#include "rfb/FrameBuffer.h"
#include "Win8CursorShape.h"
#include "UpdateKeeper.h"
#include "UpdateListener.h"
#include "Win8DuplicationListener.h"

class Win8ScreenDriverImpl : private GuiThread, private Win8DuplicationListener
{
public:
  Win8ScreenDriverImpl(LogWriter *log, UpdateKeeper *updateKeeper,
                       LocalMutex *fbLocalMutex,
                       UpdateListener *updateListener, bool detectionEnabled = false);
  virtual ~Win8ScreenDriverImpl();

  void executeDetection();
  void terminateDetection();

  bool grabFb(const Rect *rect);

  virtual FrameBuffer *getScreenBuffer();

  // Updates destination (*dst) cursor shape properties and data.
  void updateCursorShape(CursorShape *dst);
  Point getCursorPosition();

  bool isValid();

protected:
  virtual void execute();
  virtual void onTerminate();

private:
  // Implementions of the Win8DuplicationListener listener functions.
  virtual void onFrameBufferUpdate(const Region *changedRegion);
  virtual void onCopyRect(const Rect *dstRect, int srcX, int srcY);
  virtual void onCursorPositionChanged(int x, int y);
  virtual void onCursorShapeChanged();
  virtual void onRecoverableError(const TCHAR *reason);
  virtual void onCriticalError(const TCHAR *reason);

  void initDxgi();

  // This function always return the DX DXGI_FORMAT_B8G8R8A8_UNORM format in the PixelFormat type.
  PixelFormat getDxPixelFormat() const;

  LogWriter *m_log;

  ThreadCollector m_deskDuplThreadBundle;

  WindowsEvent m_initEvent;
  WindowsEvent m_errorEvent;

  // The duplication interface can't be used
  bool m_hasCriticalError;
  // The interface can be used but it should be reinitialized.
  bool m_hasRecoverableError;

  // The frame buffer with appropriate properties creates once at the constructor time. And then
  // has these properties permanently.
  FrameBuffer m_frameBuffer;

  // Cursor's properties changes at all time. And then it should be safe by a local mutex.
  Point m_latestCursorPos;
  Win8CursorShape m_win8CursorShape;
  LONGLONG m_curTimeStamp;
  LocalMutex m_cursorMutex;

  UpdateKeeper *m_updateKeeper;
  UpdateListener *m_updateListener;
  bool m_detectionEnabled;
};

#endif // __WIN8SCREENDRIVERIMPL_H__
