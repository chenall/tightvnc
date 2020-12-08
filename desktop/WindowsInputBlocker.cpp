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

#include "WindowsInputBlocker.h"
#include "util/Exception.h"
#include "thread/AutoLock.h"

LocalMutex WindowsInputBlocker::m_instanceMutex;
HHOOK WindowsInputBlocker::m_hKeyboardHook = 0;
HHOOK WindowsInputBlocker::m_hSoftKeyboardHook = 0;
HHOOK WindowsInputBlocker::m_hMouseHook = 0;
HHOOK WindowsInputBlocker::m_hSoftMouseHook = 0;

DateTime WindowsInputBlocker::m_lastInputTime;
unsigned int WindowsInputBlocker::m_timeInterval = INFINITE;
LocalMutex WindowsInputBlocker::m_lastInputTimeMutex;

WindowsInputBlocker *WindowsInputBlocker::m_instance = 0;

WindowsInputBlocker::WindowsInputBlocker(LogWriter *log)
: m_isKeyboardBlocking(false),
  m_isMouseBlocking(false),
  m_isSoftKeyboardBlocking(false),
  m_isSoftMouseBlocking(false),
  m_log(log)
{
  {
    AutoLock al(&m_instanceMutex);
    if (m_instance != 0) {
      throw Exception(_T("The only one instance of")
                      _T("WindowsInputBlocker is allowed"));
    }
    m_instance = this;
  }
  resume();
}

WindowsInputBlocker::~WindowsInputBlocker()
{
  terminate();
  wait();
  m_instance = 0;
}

DateTime WindowsInputBlocker::getLastInputTime() const
{
  AutoLock al(&m_lastInputTimeMutex);
  return m_lastInputTime;
}

void WindowsInputBlocker::correctLastTime(DateTime newTime)
{
  AutoLock al(&m_lastInputTimeMutex);
  if (newTime.getTime() > m_lastInputTime.getTime()) {
    newTime = DateTime(newTime.getTime());
  }
}

bool WindowsInputBlocker::isRemoteInputAllowed()
{
  if ((DateTime::now() - m_lastInputTime).getTime() < m_timeInterval
      && (m_hSoftKeyboardHook != 0 || m_hSoftMouseHook != 0)) {
    return false;
  }

  return true;
}

void WindowsInputBlocker::setKeyboardBlocking(bool block)
{
  m_isKeyboardBlocking = block;
  PostThreadMessage(getThreadId(), 0, 0, 0);
}

void WindowsInputBlocker::setMouseBlocking(bool block)
{
  m_isMouseBlocking = block;
  PostThreadMessage(getThreadId(), 0, 0, 0);
}

void WindowsInputBlocker::setSoftKeyboardBlocking(bool block, unsigned int timeInterval)
{
  if (block) {
    m_timeInterval = timeInterval;
  }
  m_isSoftKeyboardBlocking = block;
  PostThreadMessage(getThreadId(), 0, 0, 0);
}

void WindowsInputBlocker::setSoftMouseBlocking(bool block, unsigned int timeInterval)
{
  if (block) {
    m_timeInterval = timeInterval;
  }
  m_isSoftMouseBlocking = block;
  PostThreadMessage(getThreadId(), 0, 0, 0);
}

bool WindowsInputBlocker::setKeyboardFilterHook(bool block)
{
  if (block) {
    if (m_hKeyboardHook == 0) {
      HINSTANCE hinst = GetModuleHandle(0);
      m_hKeyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, (HOOKPROC)lowLevelKeyboardFilterProc, hinst, 0L);
    }
    return m_hKeyboardHook != 0;

  } else {
    if (m_hKeyboardHook != 0) {
      if (UnhookWindowsHookEx(m_hKeyboardHook) == FALSE) {
        return false;
      }
      m_hKeyboardHook = 0;
    }
  }
  return true;
}

bool WindowsInputBlocker::setSoftKeyboardFilterHook(bool block)
{
  if (block) {
    if (m_hSoftKeyboardHook == 0) {
      HINSTANCE hinst = GetModuleHandle(0);
      m_hSoftKeyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, (HOOKPROC)lowLevelSoftKeyboardFilterProc, hinst, 0L);
    }
    return m_hSoftKeyboardHook != 0;

  } else {
    if (m_hSoftKeyboardHook != 0) {
      if (UnhookWindowsHookEx(m_hSoftKeyboardHook) == FALSE) {
        return false;
      }
      m_hSoftKeyboardHook = 0;
    }
  }
  return true;
}

bool WindowsInputBlocker::setMouseFilterHook(bool block)
{
  if (block) {
    if (m_hMouseHook == 0) {
      HINSTANCE hinst = GetModuleHandle(0);
      m_hMouseHook = SetWindowsHookEx(WH_MOUSE_LL, (HOOKPROC)lowLevelMouseFilterProc, hinst, 0L);
    }
    return m_hMouseHook != 0;

  } else {
    if (m_hMouseHook != 0) {
      if (UnhookWindowsHookEx(m_hMouseHook) == FALSE) {
        return false;
      }
      m_hMouseHook = 0;
    }
  }
  return true;
}

bool WindowsInputBlocker::setSoftMouseFilterHook(bool block)
{
  if (block) {
    if (m_hSoftMouseHook == 0) {
      HINSTANCE hinst = GetModuleHandle(0);
      m_hSoftMouseHook = SetWindowsHookEx(WH_MOUSE_LL, (HOOKPROC)lowLevelSoftMouseFilterProc, hinst, 0L);
    }
    return m_hSoftMouseHook != 0;

  } else {
    if (m_hSoftMouseHook != 0) {
      if (UnhookWindowsHookEx(m_hSoftMouseHook) == FALSE) {
        return false;
      }
      m_hSoftMouseHook = 0;
    }
  }
  return true;
}

LRESULT CALLBACK WindowsInputBlocker::lowLevelKeyboardFilterProc(int nCode, WPARAM wParam, LPARAM lParam)
{
  if (nCode == HC_ACTION) {
    KBDLLHOOKSTRUCT *hookStruct = (KBDLLHOOKSTRUCT *)lParam;
    // If this a hardware event then block it.
    if (!(hookStruct->flags & LLKHF_INJECTED)) {
      return TRUE;
    }
  }
  return CallNextHookEx(m_hKeyboardHook, nCode, wParam, lParam);
}

LRESULT CALLBACK WindowsInputBlocker::lowLevelMouseFilterProc(int nCode, WPARAM wParam, LPARAM lParam)
{
  if (nCode == HC_ACTION) {
    MSLLHOOKSTRUCT *hookStruct = (MSLLHOOKSTRUCT *)lParam;
    // If this a hardware event then block it.
    if (!(hookStruct->flags & LLMHF_INJECTED)) {
      return TRUE;
    }
  }
  return CallNextHookEx(m_hMouseHook, nCode, wParam, lParam);
}

LRESULT CALLBACK WindowsInputBlocker::lowLevelSoftKeyboardFilterProc(int nCode, WPARAM wParam, LPARAM lParam)
{
  if (nCode == HC_ACTION) {
    KBDLLHOOKSTRUCT *hookStruct = (KBDLLHOOKSTRUCT *)lParam;
    // If this a hardware event then update software blocking time.
    if (!(hookStruct->flags & LLKHF_INJECTED)) {
      m_lastInputTime = DateTime::now();
    }
  }
  return CallNextHookEx(m_hSoftKeyboardHook, nCode, wParam, lParam);
}

LRESULT CALLBACK WindowsInputBlocker::lowLevelSoftMouseFilterProc(int nCode, WPARAM wParam, LPARAM lParam)
{
  if (nCode == HC_ACTION) {
    MSLLHOOKSTRUCT *hookStruct = (MSLLHOOKSTRUCT *)lParam;
    // If this a hardware event then update software blocking time.
    if (!(hookStruct->flags & LLMHF_INJECTED)) {
      m_lastInputTime = DateTime::now();
    }
  }
  return CallNextHookEx(m_hSoftMouseHook, nCode, wParam, lParam);
}

void WindowsInputBlocker::onTerminate()
{
  PostThreadMessage(getThreadId(), 0, 0, 0);
}

void WindowsInputBlocker::execute()
{
  m_log->info(_T("input blocker thread id = %d"), getThreadId());

  MSG msg;
  try {
    while (!isTerminating()) {
      if (m_isKeyboardBlocking && m_hKeyboardHook == 0) {
        // FIXME: write error handler
        bool res = setKeyboardFilterHook(true);
		m_log->info(_T("setKeyboardFilterHook result = %d"), res);
      }
      if (!m_isKeyboardBlocking && m_hKeyboardHook != 0) {
        // FIXME: write error handler
        setKeyboardFilterHook(false);
      }

      if (m_isMouseBlocking && m_hMouseHook == 0) {
        // FIXME: write error handler
        bool res = setMouseFilterHook(true);
		m_log->info(_T("setMouseFilterHook result = %d"), res);
      }
      if (!m_isMouseBlocking && m_hMouseHook != 0) {
        // FIXME: write error handler
        setMouseFilterHook(false);
      }

      if (m_isSoftKeyboardBlocking && m_hSoftKeyboardHook == 0) {
        // FIXME: write error handler
        bool res = setSoftKeyboardFilterHook(true);
		m_log->info(_T("setSoftKeyboardFilterHook result = %b"), res);
      }
      if (!m_isSoftKeyboardBlocking && m_hSoftKeyboardHook != 0) {
        // FIXME: write error handler
        setSoftKeyboardFilterHook(false);
      }

      if (m_isSoftMouseBlocking && m_hSoftMouseHook == 0) {
        // FIXME: write error handler
        bool res = setSoftMouseFilterHook(true);
		m_log->info(_T("setSoftMouseFilterHook result = %b"), res);
      }
      if (!m_isSoftMouseBlocking && m_hSoftMouseHook != 0) {
        // FIXME: write error handler
        setSoftMouseFilterHook(false);
      }

      if (!PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE)) {
        if (!WaitMessage()) {
          break;
        }

      } else if (msg.message == WM_QUIT) {
        break;

      } else {
        DispatchMessage(&msg);
      }
    }
  } catch (Exception &e) {
    m_log->error(_T("The WindowsInputBlocker thread failed with error: %s"),
               e.getMessage());
  }

  // Free system resources
  setKeyboardFilterHook(false);
  setMouseFilterHook(false);
  setSoftKeyboardFilterHook(false);
  setSoftMouseFilterHook(false);
}
