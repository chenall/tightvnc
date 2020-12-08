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

#include "DesktopSrvDispatcher.h"
#include "ReconnectException.h"
#include "util/CommonHeader.h"

DesktopSrvDispatcher::DesktopSrvDispatcher(BlockingGate *gate,
                                           AnEventListener *extErrorListener,
                                           LogWriter *log)
: m_gate(gate),
  m_extErrorListener(extErrorListener),
  m_log(log)
{
}

DesktopSrvDispatcher::~DesktopSrvDispatcher()
{
  terminate();
  resume();
  wait();
}

void DesktopSrvDispatcher::onTerminate()
{
}

void DesktopSrvDispatcher::notifyOnError()
{
  if (m_extErrorListener) {
    m_extErrorListener->onAnObjectEvent();
  }
}

void DesktopSrvDispatcher::execute()
{
  while (!isTerminating()) {
    try {
      m_log->debug(_T("DesktopSrvDispatcher reading code"));
      UINT8 code = m_gate->readUInt8();
      m_log->debug(_T("DesktopSrvDispatcher, code %d recieved"), code);
      std::map<UINT8, ClientListener *>::iterator iter = m_handlers.find(code);
      if (iter == m_handlers.end()) {
        StringStorage errMess;
        errMess.format(_T("Unhandled %d code has been ")
                       _T("received from a client"),
                       (int)code);
        throw Exception(errMess.getString());
      }
      (*iter).second->onRequest(code, m_gate);
    } catch (ReconnectException &) {
      m_log->message(_T("The DesktopServerApplication dispatcher has been reconnected"));
    } catch (Exception &e) {
      m_log->error(_T("The DesktopServerApplication dispatcher has ")
                 _T("failed with error: %s"), e.getMessage());
      notifyOnError();
      terminate();
    }
    Thread::yield();
  }
  m_log->message(_T("The DesktopServerApplication dispatcher has been stopped"));
}

void DesktopSrvDispatcher::registerNewHandle(UINT8 code, ClientListener *listener)
{
  m_handlers[code] = listener;
}
