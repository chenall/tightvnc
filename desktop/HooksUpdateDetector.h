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

#ifndef __HOOKSUPDATEDETECTOR_H__
#define __HOOKSUPDATEDETECTOR_H__

#include "UpdateDetector.h"
#include "win-system/WindowsEvent.h"
#include "HookInstaller.h"
#include "gui/MessageWindow.h"
#include "HookUpdateTimer.h"
#include "win-system/Process.h"
#include "log-writer/LogWriter.h"

class HooksUpdateDetector : public UpdateDetector
{
public:
  HooksUpdateDetector(UpdateKeeper *updateKeeper,
                      UpdateListener *updateListener, LogWriter *log);
  virtual ~HooksUpdateDetector();

protected:
  virtual void execute();
  virtual void onTerminate();

  void start32Loader();
  void terminate32Loader();

  WindowsEvent m_initWaiter;

  HookInstaller *m_hookInstaller;
  MessageWindow *m_targetWin;
  HookUpdateTimer m_updateTimer;
  Process m_hookLoader32;

private:
  void broadcastMessage(UINT message);

  static const TCHAR HOOK_LOADER_NAME[];

  LogWriter *m_log;
};

#endif // __HOOKSUPDATEDETECTOR_H__
