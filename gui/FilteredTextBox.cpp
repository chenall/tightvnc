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

#include "FilteredTextBox.h"

FilteredTextBox::FilteredTextBox()
: m_oldWindowProc(NULL), m_tip(NULL)
{
  m_text.setString(_T(""));
}

FilteredTextBox::~FilteredTextBox()
{
}

void FilteredTextBox::setWindow(HWND hwnd)
{
  if (hwnd != NULL) {
    m_oldWindowProc = SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG_PTR)&windowProc);
    SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)this);
  } else {
    m_oldWindowProc = NULL;
  }
  Control::setWindow(hwnd);
}

void FilteredTextBox::setText(TCHAR *text)
{
  m_text.setString(text);
  TextBox::setText(text);
}

void FilteredTextBox::setErrorBalloonTip(BalloonTip *tip)
{
  m_tip = tip;
}

void FilteredTextBox::setStringFilter(StringFilter *filter)
{
  m_filter = filter;
}

LRESULT FilteredTextBox::makeCheck()
{
  StringStorage updatedText;
  TextBox::getText(&updatedText);
  if (isStringValid(updatedText.getString())) {
    m_text = updatedText;
  } else {
    if (m_tip != NULL) {
      showBalloonTip(m_tip);
      TextBox::setText(m_text.getString());
      TextBox::selectText(m_text.getLength(), m_text.getLength());
    } else {
      //
      // TODO: Play annoying sound this
      //
    }
    return -1;
  }
  return 0;
}

bool FilteredTextBox::isStringValid(const TCHAR *string)
{
  if (m_filter != NULL) {
    return m_filter->isStringCorrect(string);
  }
  return true;
}

//
// Return values: 0 - if window process this message
//      other value - otherwise
//

LRESULT  FilteredTextBox::onKeyDown(WPARAM code, LPARAM params)
{
  return makeCheck();
}

LRESULT FilteredTextBox::windowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  FilteredTextBox *_this = (FilteredTextBox *)GetWindowLongPtr(hwnd, GWLP_USERDATA);
  if (_this == NULL) {
    return FALSE;
  }
  switch (uMsg) {
  case WM_CHAR:
    LRESULT result = CallWindowProc((WNDPROC)_this->m_oldWindowProc, hwnd, uMsg, wParam, lParam);
    return _this->onKeyDown(wParam, lParam);
  }
  return CallWindowProc((WNDPROC)_this->m_oldWindowProc, hwnd, uMsg, wParam, lParam);
}
