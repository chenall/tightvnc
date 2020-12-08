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

#ifndef __WINEVENTLOG_H__
#define __WINEVENTLOG_H__

#include "util/CommonHeader.h"
#include <vector>
#include "thread/LocalMutex.h"
#include "log-writer/LogWriter.h"

typedef std::vector<TCHAR *> StringContainer;

class WinEventLog
{
public:
  WinEventLog(LogWriter *log);
  virtual ~WinEventLog();

  // This function enables fully functional this object usage,
  // before that all report will be ignored.
  void enable();

  void reportInfo(unsigned int messageId, const TCHAR *fmt, ...);
  void reportWarning(unsigned int messageId, const TCHAR *fmt, ...);
  void reportError(unsigned int messageId, const TCHAR *fmt, ...);

private:
  void reportEvent(unsigned int messageId,
                   WORD eventType,
                   const TCHAR *fmt,
                   va_list argList);

  void registerEventSource();
  void deRegisterEventSource();
  // Updates event sources pathes to the self in the registry.
  void updateEventSourcesSubkey();

  HANDLE getLogHandle();

  HANDLE m_hEventLog;
  LocalMutex m_hEventLogMutex;

  LogWriter *m_log;
};

#endif // __WINEVENTLOG_H__
