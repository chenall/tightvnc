// Copyright (C) 2012 GlavSoft LLC.
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

#include "LogWriter.h"
#include <cstdarg>
#include <vector>

LogWriter::LogWriter(Logger *logger)
: m_logger(logger)
{
  m_profiler = new ProfileLogger();
}

LogWriter::~LogWriter()
{
  delete m_profiler;
}

void LogWriter::interror(const TCHAR *fmt, ...)
{
  int level = LOG_INTERR;
  if (m_logger != 0 && m_logger->acceptsLevel(level)) {
    va_list vl;
    va_start(vl, fmt);
    vprintLog(level, fmt, vl);
    va_end(vl);
  }
}

void LogWriter::error(const TCHAR *fmt, ...)
{
  int level = LOG_ERR;
  if (m_logger != 0 && m_logger->acceptsLevel(level)) {
    va_list vl;
    va_start(vl, fmt);
    vprintLog(level, fmt, vl);
    va_end(vl);
  }
}

void LogWriter::warning(const TCHAR *fmt, ...)
{
  int level = LOG_WARN;
  if (m_logger != 0 && m_logger->acceptsLevel(level)) {
    va_list vl;
    va_start(vl, fmt);
    vprintLog(level, fmt, vl);
    va_end(vl);
  }
}

void LogWriter::message(const TCHAR *fmt, ...)
{
  int level = LOG_MSG;
  if (m_logger != 0 && m_logger->acceptsLevel(level)) {
    va_list vl;
    va_start(vl, fmt);
    vprintLog(level, fmt, vl);
    va_end(vl);
  }
}

void LogWriter::info(const TCHAR *fmt, ...)
{
  int level = LOG_INFO;
  if (m_logger != 0 && m_logger->acceptsLevel(level)) {
    va_list vl;
    va_start(vl, fmt);
    vprintLog(level, fmt, vl);
    va_end(vl);
  }
}

void LogWriter::detail(const TCHAR *fmt, ...)
{
  int level = LOG_DETAIL;
  if (m_logger != 0 && m_logger->acceptsLevel(level)) {
    va_list vl;
    va_start(vl, fmt);
    vprintLog(level, fmt, vl);
    va_end(vl);
  }
}

void LogWriter::debug(const TCHAR *fmt, ...)
{
  int level = LOG_DEBUG;
  if (m_logger != 0 && m_logger->acceptsLevel(level)) {
    va_list vl;
    va_start(vl, fmt);
    vprintLog(level, fmt, vl);
    va_end(vl);
  }
}

bool LogWriter::isDebug()
{
  return (m_logger != 0 && m_logger->acceptsLevel(LOG_DEBUG));
}

ProcessorTimes LogWriter::checkPoint(const TCHAR * tag)
{
  return m_profiler->checkPoint(tag);
}

#pragma warning(push)
#pragma warning(disable:4996)

void LogWriter::vprintLog(int logLevel, const TCHAR *fmt, va_list argList)
{
  if (m_logger != 0) {
    // Format the original string.
    int count = _vsctprintf(fmt, argList);
    std::vector<TCHAR> formattedString(count + 1);
    _vstprintf(&formattedString.front(), fmt, argList);

    m_logger->print(logLevel, &formattedString.front());
// #if DROP_TIME_STAT // test code
    std::vector<std::vector<TCHAR>> resultStrings = m_profiler->dropStat();
    for (size_t i = 0; i < resultStrings.size(); i++) {
      formattedString = resultStrings[i];
      if (formattedString.size() != 0)
        m_logger->print(9, &formattedString.front());
    }
// #endif
  }
}

#pragma warning(pop)
