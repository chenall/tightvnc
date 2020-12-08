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

#ifndef _TOOL_BAR_H_
#define _TOOL_BAR_H_

#include "util/CommonHeader.h"

class ToolBar
{
public:
  ToolBar();
  virtual ~ToolBar();

  static const int TB_Style_sep = 0;
  static const int TB_Style_gap = 1;

   ///////////////////////////////////////////////////////// 
  // Auto mode procedures
  /////////////////////////////////////////////////////////

  // setViewAutoButtons()
  // It used only for auto buttons creation from
  // bitmap and making gaps or separators.
  void setViewAutoButtons(int iButton, int style);

  // loadToolBarfromRes()
  // This procedure will load a toolbar image from resource
  // id is a number of bitmap. It means that buttons are
  // square(for example 16x16).
  void loadToolBarfromRes(DWORD id);

  // setButtonsRange()
  // If we want to catch the message from toolbar that some buttons
  // are pressed then we must set a range for message, we pass only
  // first item, and next is id+1 and so on.
  void setButtonsRange(DWORD id);

  // attachToolBar()
  // This one will create and attach toolbar window to 
  // which handle is passed in hwnd. Only for toolbar from bitmap.
  void attachToolBar(HWND hwnd);

  ///////////////////////////////////////////////////////// 
  // Manual mode procedures
  /////////////////////////////////////////////////////////

  // create() creates a windows toolbar. dwStyle is a combination of 
  // the toolbar control and button styles. It returns true if successful,
  // or false otherwise.
  bool create(int tbID, HWND parentHwnd, 
       DWORD dwStyle = WS_CHILD | WS_VISIBLE | TBSTYLE_FLAT);

  // addBitmap() adds one or more images from resources to
  // the list of button images available for a toolbar.
  // Returns the index of the first new image if successful,
  // or -1 otherwise.
  LRESULT addBitmap(int nButtons, UINT bitmapID);

  // addSystemBitmap() adds the system-defined button bitmaps to the list
  // of the toolbar button specifying by stdBitmapID. Returns the index of 
  // the first new image if successful, or -1 otherwise.
  LRESULT addSystemBitmap(UINT stdBitmapID);

  // addNButton() adds nButtons buttons to a toolbar.
  bool addNButton(int nButtons, LPTBBUTTON tbb);

  // addButton() adds one button.
  bool addButton(int iBitmap, int idCommand, BYTE state=TBSTATE_ENABLED, 
                 BYTE style=TBSTYLE_BUTTON,  UINT dwData=0, int iString=0);

  // checkButton() checks or unchecks a given button in a toolbar control.
  bool checkButton(int idButton, bool check);

  // enableButton() enables or disables the specified button 
  // in the toolbar.
  bool enableButton(int idButton, bool enable);

  // pressButton() presses or releases the specified button in the toolbar.
  bool pressButton(int idButton, bool press);

  // getButtonRect() gets the bounding rectangle of a button in a toolbar.
  bool getButtonRect(int nIndex, LPRECT buttonRect);

  // setButtonSize() sets the size of the buttons to be added to a toolbar.
  // Button size must be largen the button bitmap.
  bool setButtonsSize(int width, int height);

  // autoSize() resizes the toolbar window.
  void autoSize();

  // getButtonsHeight() retrieves the height of the toolbar buttons.
  int getButtonsHeight();

  // getButtonsWidth() retrieves the width of the toolbar buttons.
  int getButtonsWidth();

  // isVisible() check the toolbar window on visible.
  bool isVisible();

  // hide() hides the toolbar window.
  void hide();

  // show() displays the toolbar window.
  void show();

  // getTotalWidth() returns the total size of all buttons and 
  // separators in the toolbar.
  int getTotalWidth();

  // getHeight() returns the toolbar window height.
  int getHeight();

  // getState() gets button state
  LRESULT getState(int idButton);

private:
  int m_initialStr;
  int m_numberTB;
  DWORD m_id;
  int m_width, m_height;
  HWND m_hWndToolbar;

  std::map<int, int> m_autoButtons;
};

#endif
