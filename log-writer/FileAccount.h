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

#ifndef __FILEACCOUNT_H__
#define __FILEACCOUNT_H__

#include "util/CommonHeader.h"
#include "util/DateTime.h"
#include "thread/LocalMutex.h"
#include "file-lib/WinFile.h"
#include "LogDump.h"

class FileAccount : public LogDump
{
public:
  // @param fileName - is a clear name of a file without path and extension.
  // @param logDir - is a log directory at the FileAccount creation time.
  // @param logLevel - is a log level at the FileAccount creation time.
  // @remark To avoid collisions the only once object per application must be
  // created with the same file name.
  // @remark After the object created it will write logs to the file. Also,
  // the logs will to store in memory and then will may be used as the log header on each
  // new log file creation (if it will happen). To stop accumulate the log header it
  // is needed to call the storeHeader() function. If the file have opened on the object creation
  // immediately the accumulation will be happening parallelly to writing to the file.
  // If the storeHeader() function will be forgotten the accumulation will be stopped
  // at a maximum log header value automatically.
  FileAccount(const TCHAR *logDir, const TCHAR *fileName, unsigned char logLevel, bool logHeadEnabled);

  // This constructor is a constructor with postponed initialization.
  // This constructor can be used when the log parameters are still unknown.
  // The parameters can be initialized later by the init() function.
  // After the object created by this constructor, no files will be opened but
  // writing to the log is possible (this log information will be stored in memory
  // and then, after the init() function calling, if logLevel will be nonzero, it will
  // be stored to a file.
  FileAccount(bool logHeadEnabled);

  virtual ~FileAccount();

  // This function must to call after the constructor with postponed initialization.
  // Between the constructor and this function calling all log lines stores in
  // the dump. The init() function flushes the dump to the log file and disables
  // further dumping.
  void init(const TCHAR *logDir, const TCHAR *fileName, unsigned char logLevel);

  // Allows change log directory and/or logLevel.
  // If log level changes from zero at first time the function will make
  // old log files backup and create new file. If log level changes from zero
  // the function will not create new file and continue write to existing file.
  // If log directory has been changed the function will do like as level
  // changes from zero at first time but in new directory. If level changes to
  // zero the function closes the log file.
  void changeLogProps(const TCHAR *newLogDir, unsigned char newLevel);

  // Puts the clear file name to the *fileName argument.
  void getFileName(StringStorage *fileName);

  // Tests the fileName argument with our clear file name if they
  // are equal the function returns true else false.
  bool isTheOurFileName(const TCHAR *fileName);

  // Stores a log message to the log file if level is less or equal than
  // the log verbosity level.
  virtual void print(unsigned int processId,
                     unsigned int threadId,
                     const DateTime *dt,
                     int level,
                     const TCHAR *message);

  virtual bool acceptsLevel(int logLevel);

protected:
  virtual void flush(unsigned int processId,
                     unsigned int threadId,
                     const DateTime *dt,
                     int level,
                     const TCHAR *message);

private:
  // Changes or creates or closes file depending on a new level and
  // log directory. If no changes detected then function return without
  // doing.
  // @throw Exception on an error.
  void setNewFile(unsigned char newLevel, const TCHAR *newDir);

  // Opens a log file if backup supported or replaces/created if not.
  // If directory after twice changes is the same and level > 0 then
  // log file will be rewrited.
  // @throw Exception on an error.
  void openFile();

  // Closes the file if it is open.
  void closeFile();

  // Adds unicode signature if it is not present
  void addUnicodeSignature();

  // Creates backup files
  void createBackup(unsigned int backupLimit);

  // Formates the message and stores it to the file.
  void format(unsigned int processId,
              unsigned int threadId,
              const DateTime *dt,
              int level,
              const TCHAR *message);

  // Prints self message the same log file (if it's possible)
  void print(int level, const TCHAR *message);

  bool printsLine(int level);

  void updateLogDirPath();

  StringStorage m_logDir;
  StringStorage m_fileName;
  unsigned char m_level;
  bool m_asFirstOpen;
  WinFile *m_file;

  LocalMutex m_logMut;
};

#endif // __FILEACCOUNT_H__
