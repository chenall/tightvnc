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

#include "MessageWindow.h"

#define DEFAULT_WINDOW_CLASS_NAME "WindowClass"

MessageWindow::MessageWindow(const HINSTANCE hinst,
                             const TCHAR *windowClassName,
                             WindowMessageHandler *messageHandler)
: m_hwnd(0),
  m_hinst(hinst),
  m_windowClassName(0),
  m_messageHandler(messageHandler)
{
  if (windowClassName != 0) {
    m_windowClassName = _tcsdup(windowClassName);
  } else {
    m_windowClassName = _tcsdup(_T(DEFAULT_WINDOW_CLASS_NAME));
  }

}

MessageWindow::~MessageWindow(void)
{
  destroyWindow();

  if (m_windowClassName != 0) {
    UnregisterClass(m_windowClassName, m_hinst);
    free(m_windowClassName);
  }
}

bool MessageWindow::createWindow(WindowMessageHandler *messageHandler)
{
  if (messageHandler != 0) {
    m_messageHandler = messageHandler;
  }

  if (regClass(m_hinst, m_windowClassName) == 0) {
    return false;
  }

  m_hwnd = ::CreateWindow(m_windowClassName, _T("MessageWindow"),
                          WS_OVERLAPPEDWINDOW, 0, 0, 1, 1,
                          0, NULL, m_hinst, this);

  if (m_hwnd == 0) {
    return false;
  }

  SetWindowLongPtr(m_hwnd, GWLP_USERDATA, (LONG_PTR) this);
  return true;
}

void MessageWindow::destroyWindow()
{
  if (m_hwnd) {
    DestroyWindow(m_hwnd);
    m_hwnd = 0;
  }
}

LRESULT CALLBACK MessageWindow::staticWndProc(HWND hwnd, UINT message,
                                       WPARAM wParam, LPARAM lParam)
{
  MessageWindow *_this;
  if (message == WM_CREATE) {
    _this = (MessageWindow *)((CREATESTRUCT *)lParam)->lpCreateParams;
    wParam = (WPARAM)hwnd; // Pass hwnd throw wParam
  } else {
    _this = (MessageWindow *)GetWindowLongPtr(hwnd, GWLP_USERDATA);
  }
  if (_this != NULL) {
    bool result;
    if (_this->m_messageHandler != 0) {
      result = _this->m_messageHandler->processMessage(message,
                                                       wParam,
                                                       lParam);
    } else {
      result = _this->wndProc(message, wParam, lParam);
    }
    if (result) {
      return 0;
    }
  }

  return DefWindowProc(hwnd, message, wParam, lParam);
}

ATOM MessageWindow::regClass(HINSTANCE hinst, TCHAR *windowClassName)
{
  WNDCLASS wcWindowClass = {0};
  wcWindowClass.lpfnWndProc = staticWndProc;
  wcWindowClass.style = NULL;
  wcWindowClass.hInstance = m_hinst;
  wcWindowClass.lpszClassName = windowClassName;
  wcWindowClass.hCursor = NULL;
  wcWindowClass.hbrBackground = (HBRUSH)COLOR_WINDOW;

  return RegisterClass(&wcWindowClass);
}
