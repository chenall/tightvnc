// Copyright (C) 2011,2012 GlavSoft LLC.
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

#ifndef __WINEVENTLOGWRITER_H__
#define __WINEVENTLOGWRITER_H__

#include "NewConnectionEvents.h"
#include "win-event-log/WinEventLog.h"
#include "win-system/ApplicationCrashEvents.h"
#include "WinServiceEvents.h"

// This class provide response to events by writing to the Windows log.
class WinEventLogWriter : public NewConnectionEvents,
                          public ApplicationCrashEvents,
                          public WinServiceEvents
{
public:
  WinEventLogWriter(LogWriter *log);
  ~WinEventLogWriter();

  // This function enables fully functional this object usage,
  // before that all report will be ignored.
  virtual void enable();

  // The NewConnectionEvents implementations.
  virtual void onSuccAuth(const StringStorage *ip);
  virtual void onAuthFailed(const StringStorage *ip);
  virtual void onDisconnect(const StringStorage *message);

  // The ApplicationCrashEvents implementations.
  virtual void onCrash(const StringStorage *dumpPath);

  // The WinServiceEvents implementations.
  virtual void onSuccServiceStart();
  virtual void onFailedServiceStart(const StringStorage *reason);
  virtual void onServiceStop();

private:
  WinEventLog m_sysLog;
};

#endif // __WINEVENTLOGWRITER_H__
