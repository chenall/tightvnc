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

#ifndef __WINDOWSINPUTBLOCKER_H__
#define __WINDOWSINPUTBLOCKER_H__

#include "util/CommonHeader.h"
#include "thread/GuiThread.h"
#include "thread/LocalMutex.h"
#include "win-system/WinTimeMillis.h"
#include "win-system/WindowsEvent.h"
#include "InputBlocker.h"
#include "log-writer/LogWriter.h"

// Only one instance of this class may be created.

class WindowsInputBlocker : public InputBlocker, protected GuiThread
{
public:
  WindowsInputBlocker(LogWriter *log);
  virtual ~WindowsInputBlocker();

  // This functions set/unset blocks on a local keyboard and mouse.
  virtual void setKeyboardBlocking(bool block);
  virtual void setMouseBlocking(bool block);
  // This functions set/unset blocks on a local keyboard and mouse on the
  // timeInterval interval from a last software event generation.
  virtual void setSoftKeyboardBlocking(bool block, unsigned int timeInterval);
  virtual void setSoftMouseBlocking(bool block, unsigned int timeInterval);

  virtual DateTime getLastInputTime() const;
  virtual void correctLastTime(DateTime newTime);

  virtual bool isRemoteInputAllowed();

protected:
  virtual void execute();
  virtual void onTerminate();

  bool setKeyboardFilterHook(bool block);
  bool setSoftKeyboardFilterHook(bool block);
  bool setMouseFilterHook(bool block);
  bool setSoftMouseFilterHook(bool block);

  static LRESULT CALLBACK lowLevelKeyboardFilterProc(int nCode,
                                                     WPARAM wParam,
                                                     LPARAM lParam);
  static LRESULT CALLBACK lowLevelSoftKeyboardFilterProc(int nCode,
                                                         WPARAM wParam,
                                                         LPARAM lParam);
  static LRESULT CALLBACK lowLevelMouseFilterProc(int nCode,
                                                  WPARAM wParam,
                                                  LPARAM lParam);
  static LRESULT CALLBACK lowLevelSoftMouseFilterProc(int nCode,
                                                      WPARAM wParam,
                                                      LPARAM lParam);

  static HHOOK m_hKeyboardHook;
  static HHOOK m_hSoftKeyboardHook;
  static HHOOK m_hMouseHook;
  static HHOOK m_hSoftMouseHook;

  static WindowsInputBlocker *m_instance;
  static LocalMutex m_instanceMutex;

  bool m_isKeyboardBlocking;
  bool m_isMouseBlocking;

  bool m_isSoftKeyboardBlocking;
  bool m_isSoftMouseBlocking;

  static unsigned int m_timeInterval;
  static DateTime m_lastInputTime;
  static LocalMutex m_lastInputTimeMutex;

  LogWriter *m_log;
};

#endif // __WINDOWSINPUTBLOCKER_H__
