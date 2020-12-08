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

#include "WindowsEvent.h"
#include "util/Exception.h"

WindowsEvent::WindowsEvent(const TCHAR *name)
{
  m_hEvent = CreateEvent(0, FALSE, FALSE, name);
  if (m_hEvent == 0) {
    int errCode = GetLastError();
    StringStorage errMess;
    errMess.format(_T("Cannot create windows event with error = %d"), errCode);
    throw Exception(errMess.getString());
  }
}

WindowsEvent::~WindowsEvent()
{
  if (m_hEvent) {
    CloseHandle(m_hEvent);
  }
}

void WindowsEvent::notify()
{
  if (m_hEvent) {
    SetEvent(m_hEvent);
  }
}

void WindowsEvent::waitForEvent(DWORD milliseconds)
{
  // FIXME: Check WaitForSingleObject result to an error
  if (m_hEvent) {
    WaitForSingleObject(m_hEvent, milliseconds);
  }
}
