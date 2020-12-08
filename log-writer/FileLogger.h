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

#ifndef __FILELOGGER_H__
#define __FILELOGGER_H__

#include "FileAccount.h"
#include "log-writer/Logger.h"

// This class is an implementation of the Logger class to write the log into a file.
class FileLogger : public Logger
{
public:
  // @param logDir - a valid path to directory for log file.
  // @param fileName - a valid log file name without extension.
  // @param logLevel - is an initialization value for log verbosity level.
  // @remark After the object created it will write logs to the file. Also,
  // the logs will to store in memory and then will may be used as the log header on each
  // new log file creation (if it will happen). To stop accumulate the log header it
  // is needed to call the storeHeader() function. If the file have opened on the object creation
  // immediately the accumulation will be happening parallelly to writing to the file.
  // If the storeHeader() function will be forgotten the accumulation will be stopped
  // at a maximum log header value automatically.
  FileLogger(const TCHAR *logDir, const TCHAR *fileName, unsigned char logLevel, bool logHeadEnabled);

  // This constructor is a constructor with postponed initialization.
  // This constructor can be used when the log parameters are still unknown.
  // The parameters can be initialized later by the init() function.
  // After the object created by this constructor, no files will be opened but
  // writing to the log is possible (this log information will be stored in memory
  // and then, after the init() function calling, if logLevel will be nonzero, it will
  // be stored to a file.
  FileLogger(bool logHeadEnabled);

  virtual ~FileLogger();

  // This function must to call after the constructor with postponed initialization.
  // Between the constructor and this function calling all log lines stores in
  // the dump. The init() function flushes the dump to the log file and disables
  // further dumping.
  void init(const TCHAR *logDir, const TCHAR *fileName, unsigned char logLevel);

  // Stopping the log header accumulation and enabling log header writing.
  void storeHeader();

  // This function allows change a log directory and/or log level after
  // object creation.
  void changeLogProps(const TCHAR *newLogDir, unsigned char newLevel);

  // Stores a log line to the file.
  virtual void print(int logLevel, const TCHAR *line);

  virtual bool acceptsLevel(int logLevel);

private:
  FileAccount m_fileAccount;
};

#endif // __FILELOGGER_H__
