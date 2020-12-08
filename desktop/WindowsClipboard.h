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

#ifndef __WINDOWSCLIPBOARD_H__
#define __WINDOWSCLIPBOARD_H__

#include "util/CommonHeader.h"
#include "gui/MessageWindow.h"
#include "ClipboardListener.h"
#include "log-writer/LogWriter.h"
#include "thread/GuiThread.h"

class WindowsClipboard : protected MessageWindow, GuiThread
{
public:
  WindowsClipboard(ClipboardListener *clipboardListener, LogWriter *log);
  virtual ~WindowsClipboard(void);

  // This function replaces clipboard content by the text
  virtual bool writeToClipBoard(const TCHAR *text);

  virtual void readFromClipBoard(StringStorage *clipDest) const;

protected:
  virtual bool wndProc(UINT message, WPARAM wParam, LPARAM lParam);

  virtual void execute();
  virtual void onTerminate();

  void convertToRfbFormat(const StringStorage *source, StringStorage *dest);
  void convertFromRfbFormat(const TCHAR *source, StringStorage *dest);

  HWND m_hwndNextViewer;

  ClipboardListener *m_clipboardListener;

  LogWriter *m_log;

  static const HINSTANCE m_hinst;
};

#endif // __WINDOWSCLIPBOARD_H__
