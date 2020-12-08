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

#ifndef __SHAREDMEMORY_H__
#define __SHAREDMEMORY_H__

#include "util/CommonHeader.h"

class SharedMemory
{
public:
  // @throw Exception
  SharedMemory(const TCHAR *name, size_t size);
  virtual ~SharedMemory();

  void *getMemPointer() { return m_memory; }

protected:
  // Return true if need to init
  bool createFile(const TCHAR *name, size_t size);
  void mapViewOfFile();
  void setAllAccess(HANDLE objHandle);

  void freeRes();

  HANDLE m_hToMap;
  void *m_memory;
};

#endif // __SHAREDMEMORY_H__
