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

#ifndef __RFBDISPATCHER_H__
#define __RFBDISPATCHER_H__

#include "thread/Thread.h"
#include "RfbDispatcherListener.h"
#include "util/AnEventListener.h"
#include "win-system/WindowsEvent.h"
#include <map>

class RfbDispatcher : public Thread
{
public:
  RfbDispatcher(RfbInputGate *gate,
                AnEventListener *m_extTerminationListener);
  RfbDispatcher(RfbInputGate *gate,
                WindowsEvent *terminationEvent);

  virtual ~RfbDispatcher();

  void registerNewHandle(UINT32 code, RfbDispatcherListener *listener);

protected:
  virtual void execute();
  void notifyAbTermination();

  RfbInputGate *m_gate;

  std::map<UINT32, RfbDispatcherListener *> m_handlers;

  AnEventListener *m_extTerminationListener;
  WindowsEvent *m_terminationEvent;
};

#endif // __RFBDISPATCHER_H__
