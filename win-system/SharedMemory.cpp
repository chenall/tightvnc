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

#include "SharedMemory.h"
#include "util/Exception.h"
#include <Aclapi.h>

SharedMemory::SharedMemory(const TCHAR *name, size_t size)
: m_hToMap(0),
  m_memory(0)
{
  try {
    bool needToInit = createFile(name, size);
    mapViewOfFile();
    if (needToInit) {
      memset(m_memory, 0, size);
    }
  } catch (...) {
    freeRes();
    throw;
  }
}

SharedMemory::~SharedMemory()
{
  freeRes();
}

void SharedMemory::freeRes()
{
  if (m_memory) {
    UnmapViewOfFile(m_memory);
    m_memory = 0;
  }
  if (m_hToMap) {
    CloseHandle(m_hToMap);
    m_hToMap = 0;
  }
}

bool SharedMemory::createFile(const TCHAR *name, size_t size)
{
  DWORD lowSize = size & 0xffffffff;
  DWORD highSize = (DWORD64)size >> 32 & 0xffffffff;

  m_hToMap = CreateFileMapping(INVALID_HANDLE_VALUE,  // use paging file
                               0,                     // security attributes
                               PAGE_READWRITE,        // read/write access
                               highSize,              // size: high 32-bits
                               lowSize,               // size: low 32-bits
                               name);                 // name of map object
  if (m_hToMap == NULL) {
    int errCode = GetLastError();
    StringStorage errMess;
    errMess.format(_T("Cannot create file mapping with error = %d"), errCode);
    throw Exception(errMess.getString());
  }
  // The first process to attach initializes memory
  bool needToInit = GetLastError() != ERROR_ALREADY_EXISTS;

  if (needToInit) {
    setAllAccess(m_hToMap);
  }

  return needToInit;
}

void SharedMemory::mapViewOfFile()
{
  // Get a pointer to the file-mapped shared memory
  m_memory = MapViewOfFile(m_hToMap,       // object to map view of
                           FILE_MAP_WRITE, // read/write access
                           0,              // high offset:  map from
                           0,              // low offset:   beginning
                           0);             // default: map entire file
  if (m_memory == NULL) {
    int errCode = GetLastError();
    StringStorage errMess;
    errMess.format(_T("Cannot map view of file with error = %d"), errCode);
    throw Exception(errMess.getString());
  }
}

void SharedMemory::setAllAccess(HANDLE objHandle)
{
  DWORD errorCode = SetSecurityInfo(objHandle, SE_FILE_OBJECT,
                                    DACL_SECURITY_INFORMATION, // Modify DACL
                                    0,
                                    0,
                                    0, // Pointer to DACL (0 = access to all)
                                    0);
  if (errorCode != ERROR_SUCCESS) {
    StringStorage errMess;
    errMess.format(_T("Cannot SetSecurityInfo with error = %d"), (int)errorCode);
    throw Exception(errMess.getString());
  }
}

