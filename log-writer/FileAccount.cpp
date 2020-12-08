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

#include "FileAccount.h"
#include "util/Unicode.h"
#include "file-lib/File.h"
#include "thread/AutoLock.h"
#include "file-lib/EOFException.h"

FileAccount::FileAccount(const TCHAR *logDir,
                         const TCHAR *fileName,
                         unsigned char logLevel,
                         bool logHeadEnabled)
: LogDump(logHeadEnabled, false),
  m_fileName(fileName),
  m_level(0), // Real initialization must be in the setNewFile() function
  m_asFirstOpen(true),
  m_file(0)
{
  setNewFile(logLevel, logDir);
}

FileAccount::FileAccount(bool logHeadEnabled)
: LogDump(logHeadEnabled, true), // This constructor enables writing the log to the dump.
  m_level(0), // Real initialization must be in the setNewFile() function
  m_asFirstOpen(true),
  m_file(0)
{
}

FileAccount::~FileAccount()
{
  closeFile();
}

void FileAccount::init(const TCHAR *logDir, const TCHAR *fileName, unsigned char logLevel)
{
  m_fileName.setString(fileName);

  AutoLock al(&m_logMut);
  setNewFile(logLevel, logDir);
  // The log dump now must be disabled and cleared even if logLevel is zero.
  terminateLogDumping();
}

void FileAccount::changeLogProps(const TCHAR *newLogDir,
                                 unsigned char newLevel)
{
  try {
    setNewFile(newLevel, newLogDir);
  } catch (...) { // Ignoring all exceptions
  }
}

void FileAccount::getFileName(StringStorage *fileName)
{
  *fileName = m_fileName;
}

bool FileAccount::isTheOurFileName(const TCHAR *fileName)
{
  return m_fileName.isEqualTo(fileName);
}

void FileAccount::print(unsigned int processId,
                        unsigned int threadId,
                        const DateTime *dt,
                        int level,
                        const TCHAR *message)
{
  AutoLock al(&m_logMut);

  updateLogHeaderLines(processId, threadId, dt, level, message);
  updateLogDumpLines(processId, threadId, dt, level, message);
  flush(processId, threadId, dt, level, message);
}

bool FileAccount::acceptsLevel(int logLevel)
{
  return logDumpEnabled() || logHeadEnabled() || printsLine(logLevel);
}

bool FileAccount::printsLine(int level)
{
  return m_file != 0 && level <= m_level;
}

void FileAccount::flush(unsigned int processId,
                        unsigned int threadId,
                        const DateTime *dt,
                        int level,
                        const TCHAR *message)
{
  AutoLock al(&m_logMut);

  if (printsLine(level)) {
    format(processId, threadId, dt, level, message);
  }
}

void FileAccount::print(int level, const TCHAR *message)
{
  unsigned int processId = GetCurrentProcessId();
  unsigned int threadId = GetCurrentThreadId();
  DateTime dt = DateTime::now();
  print(processId, threadId, &dt, level, message);
}

void FileAccount::format(unsigned int processId,
                         unsigned int threadId,
                         const DateTime *dt,
                         int level,
                         const TCHAR *message)
{
  // FIXME: Remove windows dependence.
  // Format the timestamp.
  StringStorage timeString(_T("[Temporary unavaliable]"));
  SYSTEMTIME st;
  dt->toUtcSystemTime(&st);
  unsigned char logBarrier;
  {
    AutoLock al(&m_logMut);
    logBarrier = m_level;
  }
  if (logBarrier < 9) {
    timeString.format(_T("%.4d-%.2d-%.2d %.2d:%.2d:%.2d"),
                      st.wYear, st.wMonth, st.wDay,
                      st.wHour, st.wMinute, st.wSecond);
  } else {
    timeString.format(_T("%.4d-%.2d-%.2d %.2d:%.2d:%.2d:%.3d"),
                      st.wYear, st.wMonth, st.wDay,
                      st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
  }

  // Choose a symbol denoting the log level.
  const TCHAR logLevelSignature[] = _T("@!*+-:    xxxxxx");
  TCHAR sig = logLevelSignature[level & 0x0F];

  // Format the final string prefixed with all the service information.
  StringStorage resultLine;
  resultLine.format(_T("[%5d/%5d] %s %c %s"),
                    processId,
                    threadId,
                    timeString.getString(),
                    sig,
                    message);
  const TCHAR badCharacters[] = { 13, 10, 0 };
  resultLine.removeChars(badCharacters, sizeof(badCharacters) / sizeof(TCHAR));

  const TCHAR endLine[] = { 13, 10 };

  // Writing string without null-termination symbol.
  if (m_file != 0) {
    m_file->write(resultLine.getString(), resultLine.getSize() - sizeof(TCHAR));
    m_file->write(endLine, sizeof(endLine));
    if (logBarrier > 9) {
      m_file->flush();
    }
  }
}

void FileAccount::setNewFile(unsigned char newLevel, const TCHAR *newDir)
{
  AutoLock al(&m_logMut);
  bool levelChanged = newLevel != m_level;
  bool levelChangedFromZero = levelChanged && m_level == 0;
  bool logDirChanged = !m_logDir.isEqualTo(newDir);
  if (!levelChanged && !logDirChanged) {
    return;
  }

  if (levelChanged && !m_asFirstOpen) {
    StringStorage message;
    message.format(_T("Log verbosity level has been changed from %d to %d"),
                   (int)m_level, (int) newLevel);
    print(1, message.getString());
  }
  if (logDirChanged && !m_asFirstOpen) {
    StringStorage message;
    message.format(_T("Log directory has been changed from \"%s\" to \"%s\""),
                   m_logDir.getString(), newDir);
    print(1, message.getString());
  }

  m_level = newLevel;
  m_logDir.setString(newDir);

  if (m_level == 0) {
    closeFile();
    return;
  }

  bool fileAlreadyOpen = m_file != 0;
  bool openningIsNeeded = !fileAlreadyOpen || logDirChanged;
  // If log directory was changed then new file creating needed instead of
  // the existing file appending.
  bool asFirstOpen = m_asFirstOpen || logDirChanged;
  m_asFirstOpen = asFirstOpen;
  if (openningIsNeeded) {
    if (fileAlreadyOpen) {
      closeFile();
    }
    openFile(); // with backup creating if needed.

    if (levelChangedFromZero && !asFirstOpen) {
      StringStorage message;
      message.format(_T("Log verbosity level has been changed from 0 to %d"),
                     (int)m_level, (int) newLevel);
      print(1, message.getString());
    }
    return;
  }
}

void FileAccount::openFile()
{
  closeFile();

  StringStorage fileName;
  fileName.format(_T("%s\\%s.log"), m_logDir.getString(),
                  m_fileName.getString());
  bool shareToRead = true;
  bool asFirstOpen = m_asFirstOpen;
  if (asFirstOpen) {
    updateLogDirPath();
    m_asFirstOpen = false;
    // Create backup files
    createBackup(5);
    // Creating file
    m_file = new WinFile(fileName.getString(), F_READ_WRITE, FM_CREATE,
                         shareToRead);
  } else {
    m_file = new WinFile(fileName.getString(), F_READ_WRITE, FM_APPEND,
                         shareToRead);
  }

  if (Unicode::isEnabled() && asFirstOpen) {
    try {
      addUnicodeSignature();
      AutoLock al(&m_logMut);
      if (getLogDumpSize() != 0) {
        // The log dump already contains the header and then is not needed to call
        // writeLogHeader().
        writeLogDump();
      } else {
        writeLogHeader();
      }
    } catch (...) {
      closeFile();
    }
  }
}

void FileAccount::closeFile()
{
  if (m_file != 0) {
    delete m_file;
    m_file = 0;
  }
}

void FileAccount::addUnicodeSignature()
{
  unsigned short firstTwoBytes = 0;
  try {
    m_file->read(&firstTwoBytes, sizeof(firstTwoBytes));
  } catch (EOFException &) {
  }
  if (firstTwoBytes != Unicode::SIGNATURE) {
    firstTwoBytes = Unicode::SIGNATURE;
    m_file->write(&firstTwoBytes, sizeof(firstTwoBytes));
  }
}

void FileAccount::createBackup(unsigned int backupLimit)
{
  StringStorage oldName, newName;
  TCHAR fmt[] = _T("%s\\%s.%d.log");
  // Shift backup files
  for (int i = backupLimit - 1; i > 0; i--) {
    // Generate valid backup names
    oldName.format(fmt, m_logDir.getString(), m_fileName.getString(), i);
    newName.format(fmt, m_logDir.getString(), m_fileName.getString(), i + 1);
    File::renameTo(newName.getString(), oldName.getString());
  }
  // Copy log file to backup
  oldName.format(_T("%s\\%s.log"), m_logDir.getString(), m_fileName.getString());
  newName.format(fmt, m_logDir.getString(), m_fileName.getString(), 1);
  File::renameTo(newName.getString(), oldName.getString());
}

void FileAccount::updateLogDirPath()
{
  // Creating log directory if it is still no exists.
  File logDirectory(m_logDir.getString());
  logDirectory.mkdir();
}
