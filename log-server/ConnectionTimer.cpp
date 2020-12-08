// Copyright (C) 2010,2011,2012 GlavSoft LLC.
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

#include "ConnectionTimer.h"
#include "thread/AutoLock.h"

ConnectionTimer::ConnectionTimer(TimerListener *timerListener,
                                 unsigned int timeOut)
: m_timerListener(timerListener),
  m_timeOut(timeOut),
  m_signalEnabled(true)
{
  resume();
}

ConnectionTimer::~ConnectionTimer()
{
  m_signalEnabled = false;
  terminate();
  wait();
}

void ConnectionTimer::onTerminate()
{
  m_timer.notify();
}

void ConnectionTimer::execute()
{
  m_timer.waitForEvent(m_timeOut);
  if (m_signalEnabled) {
    m_timerListener->onTimeTimer();
  }
}
