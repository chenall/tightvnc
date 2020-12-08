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

#include "FileInfo.h"

FileInfo::FileInfo()
: m_sizeInBytes(0), m_lastModified(0), m_flags(0)
{
}

FileInfo::FileInfo(UINT64 size, UINT64 modTime,
                   UINT16 flags, const TCHAR *fileName)
: m_sizeInBytes(size), m_lastModified(modTime), m_flags(flags)
{
  m_fileName.setString(fileName);
}

//
// FIXME: EXECUTABLE flag isn't sets now
//

FileInfo::FileInfo(const File *file)
{
  m_sizeInBytes = file->length();
  m_lastModified = file->lastModified();
  m_flags = 0;
  if (file->isDirectory()) {
    m_flags |= FileInfo::DIRECTORY;
    m_sizeInBytes = 0;
    m_lastModified = 0;
  }
  file->getName(&m_fileName);
}

bool FileInfo::isDirectory() const
{
  return (m_flags & FileInfo::DIRECTORY) ? true : false;
}

bool FileInfo::isExecutable() const
{
  return (m_flags & FileInfo::EXECUTABLE) ? true : false;
}

void FileInfo::setLastModified(UINT64 time)
{
  m_lastModified = time;
}

void FileInfo::setSize(UINT64 size)
{
  m_sizeInBytes = size;
}

void FileInfo::setFlags(UINT16 flags)
{
  m_flags = flags;
}

void FileInfo::setFileName(const TCHAR *fileName)
{
  m_fileName.setString(fileName);
}

UINT64 FileInfo::lastModified() const
{
  return m_lastModified;
}

UINT64 FileInfo::getSize() const
{
  return m_sizeInBytes;
}

UINT16 FileInfo::getFlags() const
{
  return m_flags;
}

const TCHAR *FileInfo::getFileName() const
{
  return m_fileName.getString();
}
