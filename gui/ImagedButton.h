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

#ifndef _IMAGED_BUTTON_H_
#define _IMAGED_BUTTON_H_

#include "Control.h"
#include "ThemeLib.h"

//
// Owner draw button, that displays button with image and text.
//

class ImagedButton : public Control
{
public:
  ImagedButton();
  ~ImagedButton();

  //
  // Draws this ownder-draw button.
  // This method must be called in WM_DRAWITEM message handler of parent control
  //

  void drawItem(LPDRAWITEMSTRUCT dis);

  virtual void setWindow(HWND hwnd);

  void setIcon(HICON *icon, int width, int height);

private:

  //
  // Parameters:
  //
  // IN buttonRect - button area rectangle
  // IN isButtonPressed - flag that true if button pressed
  // IN textWidth - width of button text that will be drawn in pixels
  // IN textHeight - height of button text that will be drawn in pixels
  // IN imageWidth - width of image in pixels
  // IN imageHeight - height of image in pixels
  // OUT textRect - output text rectangle
  // OUT imageRect - output image rectangle
  //

  void calcRect(RECT* buttonRect, bool isButtonPressed,
                DWORD textWidth, DWORD textHeight,
                DWORD imageWidth, DWORD imageHeight,
                RECT *textRect, RECT* imageRect);

  void drawIcon(HDC* dc, RECT* imageRect, bool isPressed, bool isDisabled);
protected:
  bool m_isUsingTheme;
  bool m_mouseOver;
  HTHEME m_theme;

  //
  // Icon to display
  //

  HICON *m_icon;

  int m_iconWidth;
  int m_iconHeight;
private:
  static LRESULT CALLBACK wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
};

#endif
