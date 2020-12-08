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

#include "NamedPipe.h"
#include "util/Exception.h"
#include <crtdbg.h>
#include "win-system/Environment.h"
#include "thread/AutoLock.h"

#define MAX_PORTION_SIZE 512 * 1024

NamedPipe::NamedPipe(HANDLE hPipe, unsigned int maxPortionSize, bool asServer)
: Pipe(maxPortionSize),
  m_hPipe(hPipe),
  m_asServer(asServer)
{
}

NamedPipe::~NamedPipe(void)
{
  try {
    close();
  } catch (...) {
  }
}

void NamedPipe::close()
{
  AutoLock al(&m_hPipeMutex);
  if (m_asServer) {
    if (DisconnectNamedPipe(m_hPipe) == 0) {
      DWORD errCode = GetLastError();
      StringStorage errMess;
      errMess.format(_T("DisconnectNamedPipe failed, error code = %u"), errCode);
      throw Exception(errMess.getString());
    }
  }

  if (m_hPipe != INVALID_HANDLE_VALUE) {
    CloseHandle(m_hPipe);
    m_hPipe = INVALID_HANDLE_VALUE;
  }
  // Unblock a blocked operation
  m_readEvent.notify();
  m_writeEvent.notify();
}

size_t NamedPipe::write(const void *buffer, size_t len)
{
  return writeByHandle(buffer, len, m_hPipe);
}

size_t NamedPipe::read(void *buffer, size_t len)
{
  return readByHandle(buffer, len, m_hPipe);
}

HANDLE NamedPipe::getHandle() const
{
  return m_hPipe;
}

void NamedPipe::checkPipeHandle()
{
  if (m_hPipe == INVALID_HANDLE_VALUE) {
    throw IOException(_T("Invalid pipe handle"));
  }
}
