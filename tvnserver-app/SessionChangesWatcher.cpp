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

#include "SessionChangesWatcher.h"
#include "win-system/WTS.h"

SessionChangesWatcher::SessionChangesWatcher(AnEventListener *extSessionChangesListener,
                                             LogWriter *log)
: m_extSessionChangesListener(extSessionChangesListener),
  m_log(log)
{
  ProcessIdToSessionId(GetCurrentProcessId(), &m_baseSessionId);
  resume();
}

SessionChangesWatcher::~SessionChangesWatcher()
{
  terminate();
  wait();
}

void SessionChangesWatcher::execute()
{
  DWORD prevSession = m_baseSessionId;
  StringStorage prevDeskName, currDeskName;
  DesktopSelector::getThreadDesktopName(&prevDeskName);

  while (!isTerminating()) {
    DWORD currSessionId = WTS::getActiveConsoleSessionId(m_log);
    bool sessionChanged = prevSession != currSessionId;
    bool desktopInfoIsAvailable = DesktopSelector::getCurrentDesktopName(&currDeskName);
    bool desktopChanged = !currDeskName.isEqualTo(&prevDeskName);
    if (sessionChanged || desktopChanged || !desktopInfoIsAvailable) {
      m_log->debug(_T("Session or desktop has been changed.")
                   _T(" The process session = %u, current session = %u")
                   _T(" The process desktop = %s, current desktop = %s"),
                   (unsigned int)prevSession, (unsigned int)currSessionId,
                   prevDeskName.getString(), currDeskName.getString());
      prevSession = currSessionId;
      prevDeskName = currDeskName;
      m_extSessionChangesListener->onAnObjectEvent();
      terminate();
    } else {
      // FIXME: Use WindowsEvent instead of Sleep().
      Sleep(100);
    }
  }
}
