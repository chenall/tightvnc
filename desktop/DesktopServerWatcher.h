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

#ifndef __DESKTOPSERVERWATCHER_H__
#define __DESKTOPSERVERWATCHER_H__

#include "thread/Thread.h"
#include "win-system/Process.h"
#include "log-writer/LogWriter.h"
#include "desktop-ipc/ReconnectionListener.h"

/**
 * Thread that used to execute desktop server application.
 *
 * @behavour:
 *   Thread in infinity loop executes (and waits until it dies) desktop
 *   server application.
 *   It will break only if thread will be terminated.
 */
class DesktopServerWatcher : public Thread
{
public:
  DesktopServerWatcher(ReconnectionListener *recListener, LogWriter *log);
  virtual ~DesktopServerWatcher();

protected:
  virtual void execute();
  virtual void onTerminate();

  // Runs desktop server with XP trick if it's needed.
  void start();

  // Changes active physical session to session0 and locks workstation.
  // @throws SystemException on fail.
  void doXPTrick();

  Process *m_process;
  ReconnectionListener *m_recListener;

  LogWriter *m_log;
};

#endif // __DESKTOPSERVERWATCHER_H__
