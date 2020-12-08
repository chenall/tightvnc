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

#ifndef __CONNECTIONTIMER_H__
#define __CONNECTIONTIMER_H__

#include "thread/Thread.h"
#include "win-system/WindowsEvent.h"
#include "TimerListener.h"

// This class waits for time interval after that notifies to external listener.
// If destructor will be called early than a time interval be out then
// ConnectionTimer will not notify to external listener.
class ConnectionTimer : private Thread
{
public:
  ConnectionTimer(TimerListener *timerListener, unsigned int timeOut);
  virtual ~ConnectionTimer();

private:
  virtual void execute();
  virtual void onTerminate();

  TimerListener *m_timerListener;
  unsigned int m_timeOut;
  WindowsEvent m_timer;
  bool m_signalEnabled;
};

#endif // __CONNECTIONTIMER_H__
