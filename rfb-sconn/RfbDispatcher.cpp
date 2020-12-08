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

#include "RfbDispatcher.h"

RfbDispatcher::RfbDispatcher(RfbInputGate *gate,
                             AnEventListener *extTerminationListener)
: m_gate(gate),
  m_extTerminationListener(extTerminationListener),
  m_terminationEvent(0)
{
}

RfbDispatcher::RfbDispatcher(RfbInputGate *gate,
                             WindowsEvent *terminationEvent)
: m_gate(gate),
  m_extTerminationListener(0),
  m_terminationEvent(terminationEvent)
{
}

RfbDispatcher::~RfbDispatcher()
{
  terminate();
  resume();
  wait();
}

void RfbDispatcher::notifyAbTermination()
{
  if (m_extTerminationListener) {
    m_extTerminationListener->onAnObjectEvent();
  }
  if (m_terminationEvent) {
    m_terminationEvent->notify();
  }
}

void RfbDispatcher::execute()
{
  try {
    while (!isTerminating()) {
      UINT32 code = m_gate->readUInt8();
      if (code == 0xfc) { // special TightVNC code
        code = code << 24;
        code += m_gate->readUInt8() << 16;
        code += m_gate->readUInt8() << 8;
        code += m_gate->readUInt8();
      }
      std::map<UINT32, RfbDispatcherListener *>::iterator iter = m_handlers.find(code);
      if (iter == m_handlers.end()) {
        StringStorage errMess;
        errMess.format(_T("unhandled %d code has been received from a client"),
                       (int)code);
        throw Exception(errMess.getString());
      }
      (*iter).second->onRequest(code, m_gate);
    }
  } catch (...) {
  }
  notifyAbTermination();
}

void RfbDispatcher::registerNewHandle(UINT32 code, RfbDispatcherListener *listener)
{
  m_handlers[code] = listener;
}
