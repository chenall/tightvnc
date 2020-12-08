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

#include "WinFile.h"
#include "win-system/SystemException.h"
#include "win-system/Environment.h"
#include "EOFException.h"

WinFile::WinFile()
: m_hFile(INVALID_HANDLE_VALUE)
{
}

WinFile::WinFile(const TCHAR *pathToFile,
                 DesiredAccess dAcc,
                 FileMode fMode,
                 bool shareToRead)
: m_hFile(INVALID_HANDLE_VALUE)
{
  open(pathToFile, dAcc, fMode, shareToRead);
}

WinFile::~WinFile()
{
  close();
}

void WinFile::open(const TCHAR *pathToFile,
                   DesiredAccess dAcc,
                   FileMode fMode,
                   bool shareToRead)
{
  m_pathToFile.setString(pathToFile);

  DWORD desiredAccess = 0;
  switch (dAcc) {
  case F_READ:
    desiredAccess = GENERIC_READ;
    break;
  case F_WRITE:
    desiredAccess = GENERIC_WRITE;
    break;
  case F_READ_WRITE:
    desiredAccess = GENERIC_WRITE | GENERIC_READ;
    break;
  default:
    _ASSERT(0);
  }

  DWORD creationDisposition;
  bool append = false;
  switch (fMode) {
  case FM_APPEND:
    // creationDisposition is such as FM_OPEN_OR_CREATE
    append = true;
  case FM_OPEN_OR_CREATE:
    creationDisposition = OPEN_ALWAYS;
    break;
  case FM_CREATE:
    creationDisposition = CREATE_ALWAYS;
    break;
  case FM_CREATE_NEW:
    creationDisposition = CREATE_NEW;
    break;
  case FM_OPEN:
    creationDisposition = OPEN_EXISTING;
    break;
  case FM_TRUNCATE:
    creationDisposition = TRUNCATE_EXISTING;
    break;
  default:
    _ASSERT(0);
  }

  DWORD shareMode = 0;
  if (shareToRead) {
    shareMode |= FILE_SHARE_READ;
  }

  m_hFile = CreateFile(m_pathToFile.getString(),
                       desiredAccess,
                       shareMode,
                       0,
                       creationDisposition,
                       FILE_ATTRIBUTE_NORMAL,
                       0);
  if (!isValid()) {
    throw SystemException();
  }

  if (append) {
    // Move file pointer to the end of file.
    DWORD result = SetFilePointer(m_hFile, 0, 0, FILE_END);
    DWORD errCode = GetLastError();
    // Checking for an error
    if (result == INVALID_SET_FILE_POINTER && errCode != NO_ERROR) {
      throw SystemException(errCode);
    }
  }
}

void WinFile::close()
{
  if (isValid()) {
    CloseHandle(m_hFile);
    m_hFile = INVALID_HANDLE_VALUE;
  }
}

bool WinFile::isValid()
{
  return m_hFile != INVALID_HANDLE_VALUE;
}

void WinFile::getPathName(StringStorage *pathName)
{
  *pathName = m_pathToFile;
}

void WinFile::seek(INT64 n)
{
  LARGE_INTEGER fileSize;
  GetFileSizeEx(m_hFile, &fileSize);
  if (fileSize.QuadPart < n) {
    throw Exception(_T("Specified file pointer position is more than file length"));
  }

  LARGE_INTEGER li;
  li.QuadPart = n;
  li.LowPart = SetFilePointer(m_hFile, li.LowPart, &li.HighPart, FILE_CURRENT);
  if (li.LowPart == INVALID_SET_FILE_POINTER && GetLastError() != NO_ERROR) {
    throw SystemException(_T("Cannot set file pointer to specified position"));
  }
}

void WinFile::flush()
{
  if (!FlushFileBuffers(m_hFile)) {
    throw SystemException(_T("Cannot flush file"));
  }
}

size_t WinFile::read(void *buff, size_t count)
{
  DWORD count32 = (DWORD)count;
  _ASSERT(count == count32);
  if (count != count32) {
    throw IOException(_T("Requested size to read is too big"));
  }
  DWORD result = 0;
  if (ReadFile(m_hFile, buff, count32, &result, 0) == 0) {
    DWORD error = GetLastError();
    if (error == ERROR_HANDLE_EOF) {
      throw EOFException();
    } else {
      StringStorage errText;
      Environment::getErrStr(&errText);
      throw IOException(errText.getString());
    }
  }
  if (result == 0) {
    throw EOFException();
  }

  return result;
}

size_t WinFile::write(const void *buff, size_t count)
{
  DWORD count32 = (DWORD)count;
  _ASSERT(count == count32);
  if (count != count32) {
    throw IOException(_T("Requested size to write is too big"));
  }
  DWORD result = 0;
  if (WriteFile(m_hFile, buff, count32, &result, 0) == 0) {
    StringStorage errText;
    Environment::getErrStr(&errText);
    throw IOException(errText.getString());
  }
  return result;
}

size_t WinFile::available() {
  LARGE_INTEGER fileSize;
  GetFileSizeEx(m_hFile, &fileSize);
  size_t pos = SetFilePointer(m_hFile, 0, 0, FILE_CURRENT);
  return fileSize.QuadPart - pos;
}
