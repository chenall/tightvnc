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

#ifndef __USERINPUT_H__
#define __USERINPUT_H__

#include "util/CommonHeader.h"
#include "region/Point.h"
#include "desktop-ipc/BlockingGate.h"
#include "region/Rect.h"
#include "region/Region.h"

// This class will be an abstract interface for user input such as keyboard,
// mouse pointer, e.t.c., on the server side.
class UserInput
{
public:
  UserInput();
  virtual ~UserInput();

  // FIXME: It's no good idea to place this function to here.
  // Because it uses only for the UserInputClient class.
  virtual void sendInit(BlockingGate *gate) {}

  // Client to server user inputs
  virtual void setNewClipboard(const StringStorage *newClipboard) = 0;
  // By the keyFlag argument will be set the mouse button state as described in
  // the rfb protocol.
  virtual void setMouseEvent(const Point *newPos, UINT8 keyFlag) = 0;
  virtual void setKeyboardEvent(UINT32 keySym, bool down) = 0;
  virtual void getCurrentUserInfo(StringStorage *desktopName,
                                  StringStorage *userName) = 0;

  virtual void getPrimaryDisplayCoords(Rect *rect) = 0;
  virtual void getDisplayNumberCoords(Rect *rect,
                                      unsigned char dispNumber) = 0;
  virtual void getNormalizedRect(Rect *rect) = 0;

  virtual void getWindowCoords(HWND hwnd, Rect *rect) = 0;
  virtual HWND getWindowHandleByName(const StringStorage *windowName) = 0;

  virtual void getApplicationRegion(unsigned int procId, Region *region) = 0;
  virtual bool isApplicationInFocus(unsigned int procId) = 0;

};

#endif // __USERINPUT_H__
