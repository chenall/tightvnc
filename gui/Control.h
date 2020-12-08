// Copyright (C) 2008,2009,2010,2011,2012 GlavSoft LLC.
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

#ifndef __CONTROL_H_
#define __CONTROL_H_

#include "util/CommonHeader.h"
#include "util/StringStorage.h"

enum VerticalAlignment
{
  Left = 0x0,
  Center = 0x1,
  Right = 0x2
};

//
// Base class to control windows control
//

class Control
{
public:
  Control();
  Control(HWND hwnd);
  virtual ~Control();

  //
  // Sets HWND associated with this control
  //

  void setWindow(HWND hwnd);

  //
  // Changes enable state of this control
  //

  virtual void setEnabled(bool enabled);

  //
  // Sets text associated with window
  //

  virtual void setText(const TCHAR *text);
  virtual void setSignedInt(int value);
  virtual void setUnsignedInt(unsigned int value);

  //
  // Sets text vertical alignment
  //

  void setTextVerticalAlignment(VerticalAlignment align);

  //
  // Sets input focus to this control
  //

  void setFocus();

  //
  // Return true if window has input focus
  //

  bool hasFocus();

  //
  // Puts this control foreground and activates it
  //

  bool setForeground();

  //
  // Changes visible state of this control
  //

  void setVisible(bool visible);

  //
  // Checks if this control is active (not disabled)
  //

  virtual bool isEnabled();

  //
  // Invalidates control
  //

  void invalidate();

  //
  // Gets text associated with window
  //

  virtual void getText(StringStorage *storage);

  //
  // Returns text vertical alignment
  //

  VerticalAlignment getTextVerticalAlignment();

  //
  // Gets HWND associated with this control
  //

  HWND getWindow();

  void operator= (const HWND& window) 
  {
    setWindow(window);
  };

protected:

  //
  // Sets windows style for this control
  //

  void setStyle(DWORD styleFlags);

  //
  // Gets windows style for this control
  //

  DWORD getStyle();

  //
  // Adds style to this control
  //

  void addStyle(DWORD styleFlag);

  //
  // Removes style from this control
  //

  void removeStyle(DWORD styleFlag);

  //
  // Returns true if styleFlags parameter is in
  // this control's style
  //

  bool isStyleEnabled(DWORD styleFlags);

  //
  // Private methods for manipulation window extended styles
  //

  void setExStyle(DWORD style);
  void addExStyle(DWORD styleFlag);
  void removeExStyle(DWORD styleFlag);

  DWORD getExStyle();
  bool isExStyleEnabled(DWORD styleFlag);

  //
  // Replaces default window proc with wndProc.
  //
  // This method must be called by child classes if needs to
  // override default window behavour.
  //

  void replaceWindowProc(WNDPROC wndProc);

protected:

  //
  // Handle of window
  //

  HWND m_hwnd;

  //
  // Pointer to default window procedure
  //

  WNDPROC m_defWindowProc;
};

#endif
