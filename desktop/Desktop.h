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

#ifndef __DESKTOP_H__
#define __DESKTOP_H__

#include "util/StringStorage.h"
#include "region/Dimension.h"
#include "region/Region.h"
#include "rfb/PixelFormat.h"
#include "rfb/FrameBuffer.h"
#include "fb-update-sender/UpdateRequestListener.h"

// This class is a public interface to a desktop.
class Desktop : public UpdateRequestListener
{
public:
  virtual ~Desktop() {}

  // Puts a current desktop name from working session to the
  // desktopName argument and an user name to userMame.
  virtual void getCurrentUserInfo(StringStorage *desktopName,
                                  StringStorage *userName) = 0;
  // Puts the current frame buffer dimension and pixel format to
  // the dim and pf function arguments.
  virtual void getFrameBufferProperties(Dimension *dim, PixelFormat *pf) = 0;

  virtual void getPrimaryDesktopCoords(Rect *rect) = 0;
  // Returns a rect that is normilized from "virtual desktop" to frame buffer coordinates.
  virtual void getNormalizedRect(Rect *rect) = 0;
  virtual void getDisplayNumberCoords(Rect *rect,
                                      unsigned char dispNumber) = 0;
  virtual void getWindowCoords(HWND hwnd, Rect *rect) = 0;
  virtual HWND getWindowHandleByName(const StringStorage *windowName) = 0;

  virtual void getApplicationRegion(unsigned int procId, Region *region) = 0;
  virtual bool isApplicationInFocus(unsigned int procId) = 0;

  virtual void setKeyboardEvent(UINT32 keySym, bool down) = 0;
  virtual void setMouseEvent(UINT16 x, UINT16 y, UINT8 buttonMask) = 0;
  virtual void setNewClipText(const StringStorage *newClipboard) = 0;

  // Updates external frame buffer pixels only for the region from view port
  // located at the place in a central frame buffer.
  // If view port is out of central frame buffer bounds the function will return false.
  virtual bool updateExternalFrameBuffer(FrameBuffer *fb, const Region *region,
                                         const Rect *viewPort) = 0;
};

#endif // __DESKTOP_H__
