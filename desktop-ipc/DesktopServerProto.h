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

#ifndef __DESKTOPSERVERPROTO_H__
#define __DESKTOPSERVERPROTO_H__

#include "util/inttypes.h"
#include "rfb/FrameBuffer.h"
#include "region/Region.h"
#include "region/Dimension.h"
#include "region/Point.h"
#include "util/DateTime.h"

#include "BlockingGate.h"

class DesktopServerProto
{
protected:
  DesktopServerProto(BlockingGate *forwGate);
  virtual ~DesktopServerProto();

  virtual void readPixelFormat(PixelFormat *pf,
                               BlockingGate *gate);
  virtual void sendPixelFormat(const PixelFormat *pf,
                               BlockingGate *gate);
  virtual Dimension readDimension(BlockingGate *gate);
  virtual void sendDimension(const Dimension *dim,
                             BlockingGate *gate);
  virtual Point readPoint(BlockingGate *gate);
  virtual void sendPoint(const Point *point,
                         BlockingGate *gate);
  virtual Rect readRect(BlockingGate *gate);
  virtual void sendRect(const Rect *rect,
                        BlockingGate *gate);
  virtual void sendRegion(const Region *region,
                          BlockingGate *gate);
  virtual void readRegion(Region *region,
                          BlockingGate *gate);

  void sendFrameBuffer(const FrameBuffer *srcFb, const Rect *srcRect,
                       BlockingGate *gate);
  void readFrameBuffer(FrameBuffer *dstFb, const Rect *dstRect,
                       BlockingGate *gate);

  virtual void sendNewClipboard(const StringStorage *newClipboard,
                                BlockingGate *gate);
  virtual void readNewClipboard(StringStorage *newClipboard,
                                BlockingGate *gate);
  virtual void sendNewPointerPos(const Point *newPos, UINT8 keyFlag,
                                 BlockingGate *gate);
  virtual void readNewPointerPos(Point *newPos, UINT8 *keyFlag,
                                 BlockingGate *gate);
  virtual void sendKeyEvent(UINT32 keySym, bool down,
                            BlockingGate *gate);
  virtual void readKeyEvent(UINT32 *keySym, bool *down,
                            BlockingGate *gate);
  virtual void sendUserInfo(const StringStorage *desktopName,
                            const StringStorage *userName,
                            BlockingGate *gate);
  virtual void readUserInfo(StringStorage *desktopName,
                            StringStorage *userName,
                            BlockingGate *gate);
  virtual void sendConfigSettings(BlockingGate *gate);
  virtual void readConfigSettings(BlockingGate *gate);

  // FIXME: Remove m_forwGate from this class.
  // Forward gate will send requests
  BlockingGate *m_forwGate;

  static const UINT8 EXTRACT_REQ = 0;
  static const UINT8 SCREEN_PROP_REQ = 1;
  static const UINT8 FRAME_BUFFER_INIT = 2;
  static const UINT8 SET_FULL_UPD_REQ_REGION = 3;
  static const UINT8 SET_EXCLUDING_REGION = 4;
  static const UINT8 UPDATE_DETECTED = 10;

  static const UINT8 CLIPBOARD_CHANGED = 30;
  static const UINT8 POINTER_POS_CHANGED = 31;
  static const UINT8 KEYBOARD_EVENT = 32;
  static const UINT8 USER_INPUT_INIT = 33;
  static const UINT8 USER_INFO_REQ = 34;
  static const UINT8 DESKTOP_COORDS_REQ = 35;
  static const UINT8 WINDOW_COORDS_REQ = 36;
  static const UINT8 WINDOW_HANDLE_REQ = 37;
  static const UINT8 DISPLAY_NUMBER_COORDS_REQ = 38;
  static const UINT8 APPLICATION_REGION_REQ = 39;
  static const UINT8 NORMALIZE_RECT_REQ = 40;
  static const UINT8 APPLICATION_CHECK_FOCUS = 41;

  static const UINT8 CONFIG_RELOAD_REQ = 50;
  static const UINT8 SOFT_INPUT_ENABLING_REQ = 51;

private:
  void checkPixelFormat(const PixelFormat *pf);
  void checkRectangle(const Rect *rect);
  void checkDimension(const Dimension *dim);
};

#endif // __DESKTOPSERVERPROTO_H__
