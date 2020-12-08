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

#include "ClientLogger.h"
#include "win-system/PipeClient.h"
#include "SecurityPipeClient.h"
#include "util/DateTime.h"

ClientLogger::ClientLogger(const TCHAR *publicPipeName, const TCHAR *logFileName)
: LogDump(false, true),
  m_logSendingChan(0),
  m_levListenChan(0),
  m_logInput(0),
  m_logOutput(0),
  m_logBarrier(0),
  m_logFileName(logFileName),
  m_publicPipeName(publicPipeName)
{
}

ClientLogger::~ClientLogger()
{
  freeResources();
}

void ClientLogger::freeResources()
{
  if (m_levListenChan != 0) m_levListenChan->close();
  terminate();
  resume();
  wait();
  if (m_levListenChan != 0) delete m_levListenChan;

  if (m_logOutput != 0) delete m_logOutput;
  if (m_logInput != 0) delete m_logInput;
  if (m_logSendingChan != 0) delete m_logSendingChan;
}

void ClientLogger::connect()
{
  NamedPipe *svcChan = 0;
  try {
    // Try connect to log server
    svcChan = PipeClient::connect(m_publicPipeName.getString(), 0);
    // Try get security channel from the server.
    // Pass zero as maxPortionSize under the assumption that the pipe buffer is always greater
    // than max log line length is always.
    SecurityPipeClient secLogPipeClient(svcChan, 0);
    m_logSendingChan = secLogPipeClient.getChannel();
    m_logInput = new DataInputStream(m_logSendingChan);
    m_logOutput = new DataOutputStream(m_logSendingChan);

    SecurityPipeClient secLevelPipeClient(svcChan, 0);
    m_levListenChan = secLevelPipeClient.getChannel();

    m_logOutput->writeUTF8(m_logFileName.getString());

    // Get log level by the m_levListenChan channel.
    DataInputStream m_levInput(m_levListenChan);

    unsigned char logLevel = m_levInput.readUInt8();
    setLogBarrier(logLevel);
  } catch (Exception &e) {
    if (svcChan != 0) delete svcChan;
    freeResources();
    StringStorage formattedException;
    formattedException.format(_T("Can't connect to the log server: %s"),
                              e.getMessage());
    throw Exception(formattedException.getString());
  }
  if (svcChan != 0) delete svcChan;

  // A workaround to send first acummulated log lines even if log barrier is zero.
  // Log server must define their afterlife.
  // Maybe it can be more better coded.
  int logLevel = getLogBarrier();
  setLogBarrier(9);
  try {
    AutoLock al(&m_logWritingMut);
    writeLogDump();
  } catch (...) { // "finally"
    setLogBarrier(logLevel);
  }
  setLogBarrier(logLevel);
  terminateLogDumping();

  resume();
}

void ClientLogger::print(int logLevel, const TCHAR *line)
{
  UINT32 processId = GetCurrentProcessId();
  UINT32 threadId = GetCurrentThreadId();

  AutoLock al(&m_logWritingMut);
  updateLogDumpLines(processId, threadId, &DateTime::now(), logLevel, line);
  flush(processId, threadId, &DateTime::now(), logLevel, line);
}

bool ClientLogger::acceptsLevel(int logLevel)
{
  return logDumpEnabled() || m_logOutput != 0 && logLevel <= getLogBarrier();
}

void ClientLogger::flush(unsigned int processId,
                         unsigned int threadId,
                         const DateTime *dt,
                         int level,
                         const TCHAR *message)
{
  AutoLock al(&m_logWritingMut);

  if (level <= getLogBarrier()) {
    if (m_logOutput != 0) {
      try {
        m_logOutput->writeUInt32(processId);
        m_logOutput->writeUInt32(threadId);
        m_logOutput->writeUInt64(dt->getTime());
        m_logOutput->writeUInt8(level & 0xf);
        m_logOutput->writeUTF8(message);
      } catch (...) {
      }
    }
  }
}

int ClientLogger::getLogBarrier()
{
  AutoLock al(&m_logBarMut);
  return m_logBarrier;
}

void ClientLogger::setLogBarrier(int newLogBar)
{
  AutoLock al(&m_logBarMut);
  m_logBarrier = newLogBar & 0xf;
}

void ClientLogger::execute()
{
  DataInputStream levInput(m_levListenChan);
  try {
    while (!isTerminating()) {
      setLogBarrier(levInput.readUInt8());
    }
  } catch (...) {
  }
}
