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

#include "LogLevelSender.h"
#include "thread/AutoLock.h"

LogLevelSender::LogLevelSender()
: m_outStream(0),
  m_logLevel(0),
  m_updateAvailable(false)
{
  resume();
}

LogLevelSender::~LogLevelSender()
{
  terminate();
  wait();
}

void LogLevelSender::startSender(OutputStream *outStream)
{
  AutoLock al(&m_updateMutex);
  m_outStream = outStream;
}

void LogLevelSender::onTerminate()
{
  m_sleeper.notify();
}

void LogLevelSender::updateLevel(unsigned char newLevel)
{
  AutoLock al(&m_updateMutex);
  m_updateAvailable = true;
  m_logLevel = newLevel;
  if (m_logLevel > 10) {
    m_logLevel = 10;
  }
  m_sleeper.notify();
}

void LogLevelSender::execute()
{
  try {
    while (!isTerminating()) {
      m_sleeper.waitForEvent();
      OutputStream *outStream;
      bool updateAvailable;
      unsigned char logLevel;
      {
        AutoLock al(&m_updateMutex);
        outStream = m_outStream;
        updateAvailable = m_updateAvailable;
        m_updateAvailable = false;
        logLevel = m_logLevel;
      }
      if (outStream != 0 && updateAvailable) {
        DataOutputStream output(outStream);
        output.writeUInt8(logLevel);
      }
    }
  } catch (...) { // FIXME: Log this.
  }
}
