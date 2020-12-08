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

#ifndef __THREAD_H__
#define __THREAD_H__

#include "util/CommonHeader.h"
#include "LocalMutex.h"
#include "DesktopSelector.h"

/**
 * Thread priority enumeration.
 */
enum THREAD_PRIORITY
{
  PRIORITY_IDLE,
  PRIORITY_LOWEST,
  PRIORITY_BELOW_NORMAL,
  PRIORITY_NORMAL,
  PRIORITY_ABOVE_NORMAL,
  PRIORITY_HIGHEST,
  PRIORITY_TIME_CRITICAL
};

/**
 * Thread class.
 *
 * @fixme some of methods return bool instead of raising exceptions.
 * @fixme some methods seems to be not thread-safe (that uses m_active member).
 * @fixme member of HDESK type in THREAD class???
 */
class Thread
{
public:
  /**
   * Creates new thread.
   * @remark thread is suspended by default.
   */
  Thread();
  /**
   * Deletes thread.
   * @remark does not stops thread execution if it's still running.
   */
  virtual ~Thread();

  /**
   * Waits until thread stops.
   * @return false on error.
   */
  bool wait();
  /**
   * Suspends thread execution.
   * @return false on error.
   */
  bool suspend();
  /**
   * Resume thread execution.
   * @return false on error.
   */
  bool resume();
  /**
   * Terminates thread execution.
   * @remark thread-safe.
   */
  virtual void terminate();

  /**
   * Checks if thread is not dead.
   * @return true if thread is not dead (still running or suspended).
   */
  bool isActive() const;

  /**
   * Returns thread id.
   */
  DWORD getThreadId() const;

  /**
   * Sets thread priority.
   * @param value thread priority.
   */
  bool setPriority(THREAD_PRIORITY value);

  /**
   * Suspends the execution of the current thread until the time-out interval elapses.
   * @param millis time to sleep.
   */
  static void sleep(DWORD millis);

  /**
   * Yield execution to the next ready thread.
   */
  static void yield();

protected:
  /**
   * Returns true if terminate() method was called.
   * @remark thread-safe.
   */
  bool isTerminating();

  /**
   * Slot of terminate() signal.
   * Method called from terminate() method.
   * Can be overrided by subclasses to gracefully shutdown thread.
   */
  virtual void onTerminate();

  /**
   * Thread's runnable body.
   */
  virtual void execute() = 0;

private:
  /**
   * WinApi thread func.
   */
  static DWORD WINAPI threadProc(LPVOID pThread);

  // This function calling before call a derived execute() function to
  // perform any additional action.
  virtual void initByDerived();

private:
  /**
   * Win32 thread handle.
   */
  HANDLE m_hThread;
  /**
   * Thread ID.
   */
  DWORD m_threadID;
  /**
   * Activity flag.
   */
  bool m_active;
  /**
   * Terminating flag.
   */
  volatile bool m_terminated;
};

#endif // __THREAD_H__
