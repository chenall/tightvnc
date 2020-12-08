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

#include "Thread.h"
#include "AutoLock.h"
#include "util/Exception.h"

Thread::Thread()
: m_terminated(false), m_active(false)
{
  m_hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) threadProc,
                           (LPVOID) this, CREATE_SUSPENDED, (LPDWORD) &m_threadID);
}

Thread::~Thread()
{
  CloseHandle(m_hThread);
}

DWORD WINAPI Thread::threadProc(LPVOID pThread)
{
  Thread *_this = ((Thread *)pThread);
  try {
    _this->initByDerived();
    _this->execute();
  } catch (Exception &) {
    /*Log::error(_T("Abnormal thread termination.")
               _T(" ThreadId = %u, message = \"%s\" \n"),
               (unsigned int)_this->m_threadID, e.getMessage());

    */
  }
  _this->m_active = false;
  return 0;
}

void Thread::initByDerived()
{
  // If is needed this function will be inherited by a derived class.
}

bool Thread::wait()
{
  if (m_active) {
    return (WaitForSingleObject(m_hThread, INFINITE) != WAIT_FAILED);
  } else {
    return true;
  }
}

// FIXME: not thread-safe (m_active).
bool Thread::suspend()
{
  m_active = !(SuspendThread(m_hThread) != -1);

  return !m_active;
}

// FIXME: not thread-safe (m_active).
bool Thread::resume()
{
  m_active = ResumeThread(m_hThread) != -1;

  return m_active;
}

void Thread::terminate()
{
  m_terminated = true;

  onTerminate();
}

bool Thread::isActive() const
{
  return m_active;
}

DWORD Thread::getThreadId() const
{
  return m_threadID;
}

bool Thread::setPriority(THREAD_PRIORITY value)
{
  int priority;

  switch(value)
  {
  case PRIORITY_IDLE:
    priority = THREAD_PRIORITY_IDLE;
    break;
  case PRIORITY_LOWEST:
    priority = THREAD_PRIORITY_LOWEST;
    break;
  case PRIORITY_BELOW_NORMAL:
    priority = THREAD_PRIORITY_BELOW_NORMAL;
    break;
  case PRIORITY_NORMAL:
    priority = THREAD_PRIORITY_NORMAL;
    break;
  case PRIORITY_ABOVE_NORMAL:
    priority = THREAD_PRIORITY_ABOVE_NORMAL;
    break;
  case PRIORITY_HIGHEST:
    priority = THREAD_PRIORITY_HIGHEST;
    break;
  case PRIORITY_TIME_CRITICAL:
    priority = THREAD_PRIORITY_TIME_CRITICAL;
    break;
  default:
    priority = THREAD_PRIORITY_NORMAL;
  }

  return SetThreadPriority(m_hThread, priority) != 0;
}

void Thread::sleep(DWORD millis)
{
  Sleep(millis);
}

void Thread::yield()
{
  SwitchToThread();
}

bool Thread::isTerminating()
{
  return m_terminated;
}

void Thread::onTerminate()
{
}
