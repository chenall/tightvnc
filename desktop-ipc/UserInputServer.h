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

#ifndef __USERINPUTSERVER_H__
#define __USERINPUTSERVER_H__

#include "util/CommonHeader.h"
#include "DesktopServerProto.h"
#include "ClientListener.h"
#include "desktop/WindowsUserInput.h"
#include "win-system/WindowsEvent.h"
#include "DesktopSrvDispatcher.h"
#include "log-writer/LogWriter.h"

class UserInputServer: public DesktopServerProto, public ClientListener,
                       public ClipboardListener
{
public:
  UserInputServer(BlockingGate *forwGate,
                  DesktopSrvDispatcher *dispatcher,
                  AnEventListener *extTerminationListener,
                  LogWriter *log);
  virtual ~UserInputServer();

  // Internal dispatcher
  virtual void onRequest(UINT8 reqCode, BlockingGate *backGate);

  virtual void onClipboardUpdate(const StringStorage *newClipboard);

protected:
  virtual void applyNewPointerPos(BlockingGate *backGate);
  virtual void applyNewClipboard(BlockingGate *backGate);
  virtual void applyKeyEvent(BlockingGate *backGate);
  virtual void ansDesktopCoords(BlockingGate *backGate);
  virtual void ansWindowCoords(BlockingGate *backGate);
  virtual void ansUserInfo(BlockingGate *backGate);
  virtual void ansWindowHandle(BlockingGate *backGate);
  virtual void ansDisplayNumberCoords(BlockingGate *backGate);
  virtual void ansApplicationRegion(BlockingGate *backGate);
  virtual void ansApplicationInFocus(BlockingGate *backGate);
  virtual void ansNormalizeRect(BlockingGate *backGate);

  // At first time server must get init information.
  void serverInit(BlockingGate *backGate);

  WindowsUserInput *m_userInput;
  AnEventListener *m_extTerminationListener;

  LogWriter *m_log;
};

#endif // __USERINPUTSERVER_H__
