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

#ifndef __UPDATEHANDLERSERVER_H__
#define __UPDATEHANDLERSERVER_H__

#include "DesktopServerProto.h"
#include "desktop/UpdateHandlerImpl.h"
#include "DesktopSrvDispatcher.h"
#include "log-writer/LogWriter.h"
#include "desktop/Win32ScreenDriverFactory.h"

class UpdateHandlerServer: public DesktopServerProto, public ClientListener,
                           public UpdateListener
{
public:
  UpdateHandlerServer(BlockingGate *forwGate,
                      DesktopSrvDispatcher *dispatcher,
                      AnEventListener *extTerminationListener,
                      LogWriter *log);
  virtual ~UpdateHandlerServer();

  // Internal dispatcher
  virtual void onRequest(UINT8 reqCode, BlockingGate *backGate);

protected:
  virtual void onUpdate();

  // At first time server must get init information.
  void serverInit(BlockingGate *backGate);

  void extractReply(BlockingGate *backGate);
  void screenPropReply(BlockingGate *backGate);
  void receiveFullReqReg(BlockingGate *backGate);
  void receiveExcludingReg(BlockingGate *backGate);

  Win32ScreenDriverFactory m_scrDriverFactory;

  PixelFormat m_oldPf;

  UpdateHandlerImpl *m_updateHandler;
  AnEventListener *m_extTerminationListener;

  LogWriter *m_log;
};

#endif // __UPDATEHANDLERSERVER_H__
