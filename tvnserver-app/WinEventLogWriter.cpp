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

#include "WinEventLogWriter.h"
#include "win-event-log/MessageProvider.h"

WinEventLogWriter::WinEventLogWriter(LogWriter *log)
: m_sysLog(log)
{
}

WinEventLogWriter::~WinEventLogWriter()
{
}

void WinEventLogWriter::enable()
{
  m_sysLog.enable();
}

void WinEventLogWriter::onSuccAuth(const StringStorage *ip)
{
  m_sysLog.reportInfo(MSG_INFO_MESSAGE,
                      _T("Authentication passed by %s"),
                      ip->getString());
}

void WinEventLogWriter::onAuthFailed(const StringStorage *ip)
{
  m_sysLog.reportWarning(MSG_WARNING_MESSAGE,
                         _T("Authentication failed from %s"),
                         ip->getString());
}

void WinEventLogWriter::onDisconnect(const StringStorage *message)
{
  m_sysLog.reportInfo(MSG_INFO_MESSAGE, _T("%s"), message->getString());
}

void WinEventLogWriter::onCrash(const StringStorage *dumpPath)
{
  m_sysLog.reportError(MSG_ERROR_MESSAGE,
                       _T("Application crashed. Debug information has been saved to %s"),
                       dumpPath->getString());
}

void WinEventLogWriter::onSuccServiceStart()
{
  m_sysLog.reportInfo(MSG_INFO_MESSAGE,
                      _T("Service has been started successfully"));
}

void WinEventLogWriter::onFailedServiceStart(const StringStorage *reason)
{
  m_sysLog.reportError(MSG_ERROR_MESSAGE,
                       _T("Service has been terminated for the following reason: %s"),
                       reason->getString());
}

void WinEventLogWriter::onServiceStop()
{
  m_sysLog.reportInfo(MSG_INFO_MESSAGE, _T("Service has been stopped"));
}
