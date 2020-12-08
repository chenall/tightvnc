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

#ifndef __WINFILE_H__
#define __WINFILE_H__

#include "util/CommonHeader.h"

enum DesiredAccess
{
  F_READ,
  F_WRITE,
  F_READ_WRITE
};

enum FileMode
{
  // Opens a file, always.
  // If the specified file does not exist and is a valid path to a writable
  // location, a function creates a file.
  FM_APPEND,
  // Creates a new file, always.
  // If the specified file exists and is writable, a function overwrites
  // the file, a function succeeds.
  FM_CREATE,
  // Creates a new file, only if it does not already exist.
  // If the specified file exists, a function fails with an Exception.
  FM_CREATE_NEW,
  // Opens a file, only if it exists.
  // If the specified file does not exist, a function fails with an Exception.
  FM_OPEN,
  // Opens a file, always.
  // If the specified file does not exist and is a valid path to a writable
  // location, a function creates a file.
  FM_OPEN_OR_CREATE,
  // Opens a file and truncates it so that its size is zero bytes,
  // only if it exists.
  // If the specified file does not exist, a function fails.
  FM_TRUNCATE
};

class WinFile
{
public:
  WinFile(const TCHAR *pathToFile, DesiredAccess dAcc, FileMode fMode,
          bool shareToRead = false);
  WinFile();

  virtual ~WinFile();

  // Call this function after this object creation by the default constructor.
  void open(const TCHAR *pathToFile, DesiredAccess dAcc, FileMode fMode,
            bool shareToRead = false);

  // Closes handle to a file if handle is valid.
  void close();

  // @throw exception on a system error.
  virtual size_t read(void *buff, size_t count);

  // @throw exception on a system error.
  virtual size_t write(const void *buff, size_t count);

  // Return true if object is valid. The object will be valid if
  // after by default constructor creation the initialize function has been
  // called successfully.
  bool isValid();

  // Return valid path name to a file.
  void getPathName(StringStorage *pathName);

  // Set file pointer to specified position starting from current
  // file pointer position. Can move forward and backward.
  void seek(INT64 n);

  // @throw exception on a system error.
  void flush();

  virtual size_t available();

private:
  HANDLE m_hFile;
  StringStorage m_pathToFile;
};

#endif // __WINFILE_H__
