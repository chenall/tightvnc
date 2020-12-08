// Copyright (C) 2009,2010,2011,2012 GlavSoft LLC.
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

#ifndef _FILE_INFO_H_
#define _FILE_INFO_H_

#include "util/StringStorage.h"
#include "util/inttypes.h"
#include "file-lib/File.h"

//
// Contains information about file in file transfer protocol
// format.
//

class FileInfo
{
public:

  //
  // Combination of flags that can be used in m_flags
  // class members
  //

  const static int DIRECTORY  = 0x1;
  const static int EXECUTABLE = 0x2;

public:

  //
  // Creates FileInfo class with empty file information (no flags,
  // no size, name is set to "").
  //

  FileInfo();

  //
  // Creates FileInfo class with members with values specified
  // in constructor's arguments.
  //

  FileInfo(UINT64 size, UINT64 modTime,
           UINT16 flags, const TCHAR *fileName);

  //
  // Creates FileInfo class with name, size, flags that will be
  // retrieved from file argument.
  //

  FileInfo(const File *file);

  //
  // Returns true if DIRECTORY flag is set
  //

  bool isDirectory() const;

  //
  // Returns true if EXECUTABLE flag is set
  //

  bool isExecutable() const;

  //
  // Sets last modification time (in seconds from unix epoch)
  //

  void setLastModified(UINT64 time);

  //
  // Sets file size (in bytes)
  //

  void setSize(UINT64 size);

  //
  // Sets file flags (see static FileInfo constants)
  //

  void setFlags(UINT16 flags);

  //
  // Sets relative (from parent folder) file name
  //

  void setFileName(const TCHAR *fileName);

  //
  // Returns file last modified time (in secords, starts from unix epoch)
  //

  UINT64 lastModified() const;

  //
  // Returns file size in bytes
  //

  UINT64 getSize() const;

  //
  // Returns file flags (see FileInfo static constants)
  //

  UINT16 getFlags() const;

  //
  // Returns file name
  //

  const TCHAR *getFileName() const;

protected:
  UINT64 m_sizeInBytes;
  UINT64 m_lastModified;
  UINT16 m_flags;
  StringStorage m_fileName;
};

#endif
