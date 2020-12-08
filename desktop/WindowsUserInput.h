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

#ifndef __WINDOWSUSERINPUT_H__
#define __WINDOWSUSERINPUT_H__

#include "UserInput.h"
#include "WindowsClipboard.h"
#include "util/Keymap.h"
#include "win-system/InputInjector.h"
#include "win-system/WindowsDisplays.h"
#include "log-writer/LogWriter.h"

class WindowsUserInput : public UserInput
{
public:
  WindowsUserInput(ClipboardListener *clipboardListener,
                   bool ctrlAltDelEnabled,
                   LogWriter *log);
  virtual ~WindowsUserInput(void);

  virtual void setNewClipboard(const StringStorage *newClipboard);
  virtual void setMouseEvent(const Point *newPos, UINT8 keyFlag);
  virtual void setKeyboardEvent(UINT32 keySym, bool down);

  virtual void getCurrentUserInfo(StringStorage *desktopName,
                                  StringStorage *userName);
  virtual void getDisplayNumberCoords(Rect *rect,
                                      unsigned char dispNumber);
  virtual void getNormalizedRect(Rect *rect);
  virtual void getPrimaryDisplayCoords(Rect *rect);
  virtual void getWindowCoords(HWND hwnd, Rect *rect);
  virtual HWND getWindowHandleByName(const StringStorage *windowName);
  virtual void getApplicationRegion(unsigned int procId, Region *region);
  virtual bool isApplicationInFocus(unsigned int procId);

  virtual void initKeyFlag(UINT8 initValue) { m_prevKeyFlag = initValue; }

protected:
  void toFbCoordinates(Rect *rect);

  WindowsClipboard *m_clipboard;
  WindowsDisplays m_winDisplays;

  Keymap m_keyMap;
  InputInjector m_inputInjector;

  UINT8 m_prevKeyFlag;

  LogWriter *m_log;
};

#endif // __WINDOWSUSERINPUT_H__
