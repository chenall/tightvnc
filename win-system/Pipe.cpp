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

#include "Pipe.h"
#include "util/Exception.h"
#include <crtdbg.h>
#include "win-system/Environment.h"
#include "thread/AutoLock.h"

Pipe::Pipe(unsigned int maxPortionSize)
: m_totalRead(0),
  m_totalWrote(0),
  m_maxPortionSize(maxPortionSize)
{
}

Pipe::~Pipe()
{
}

unsigned int Pipe::getMaxPortionSize()
{
  return m_maxPortionSize;
}

size_t Pipe::writeByHandle(const void *buffer, size_t len, HANDLE pipeHandle)
{
  DWORD result;
  OVERLAPPED overlapped;
  memset(&overlapped, 0, sizeof(OVERLAPPED));
  overlapped.hEvent = m_writeEvent.getHandle();

  bool success;
  DWORD length = (DWORD)len;
  _ASSERT(length == len);
  if (m_maxPortionSize != 0 && length > m_maxPortionSize) {
    length = m_maxPortionSize;
  }
  {
    AutoLock al(&m_hPipeMutex);
    checkPipeHandle(pipeHandle);
    success = WriteFile(pipeHandle, // pipe handle
                        buffer,    // message
                        length,  // message length
                        &result, // bytes written
                        &overlapped)    // overlapped
                        != 0;
  }

  if (!success) {
    int errCode = GetLastError();

    if (errCode == ERROR_IO_PENDING) {
      m_writeEvent.waitForEvent();
      DWORD cbRet;
      AutoLock al(&m_hPipeMutex);
      checkPipeHandle(pipeHandle);
      if (GetOverlappedResult(pipeHandle, &overlapped, &cbRet, FALSE) ||
          cbRet == 0) {
        result = cbRet;
      } else {
        StringStorage errMess;
        Environment::getErrStr(_T("The Pipe's write function failed")
                               _T(" after GetOverlappedResult calling"),
                               &errMess);
        throw IOException(errMess.getString());
      }
    } else {
      StringStorage errText;
      errText.format(_T("The Pipe's write function failed after WriteFile calling")
                     _T("(pipe handle is %p, total write %llu, try to write %u)"),
                     pipeHandle, m_totalWrote, length);
      StringStorage errMess;
      Environment::getErrStr(errText.getString(), &errMess);
      throw IOException(errMess.getString());
    }
  } // else operation already successful has been completed

  if (result == 0) {
    throw IOException(_T("Unknown pipe error"));
  }

  m_totalWrote += result;
  return result;
}

size_t Pipe::readByHandle(void *buffer, size_t len, HANDLE pipeHandle)
{
  DWORD result = 0;
  OVERLAPPED overlapped;
  memset(&overlapped, 0, sizeof(OVERLAPPED));
  overlapped.hEvent = m_readEvent.getHandle();

  bool success;
  DWORD length = (DWORD)len;
  _ASSERT(length == len);
  if (m_maxPortionSize != 0 && length > m_maxPortionSize) {
    length = m_maxPortionSize;
  }
  {
    AutoLock al(&m_hPipeMutex);
    checkPipeHandle(pipeHandle);
    success = ReadFile(pipeHandle,         // pipe handle
                       buffer,            // message
                       length,          // message length
                       &result,         // bytes read
                       &overlapped)   // overlapped
                       != 0;
  }
  if (!success) {
    DWORD errCode = GetLastError();

    if (errCode == ERROR_IO_PENDING) {
      m_readEvent.waitForEvent();
      DWORD cbRet = 0;
      AutoLock al(&m_hPipeMutex);
      checkPipeHandle(pipeHandle);
      if (GetOverlappedResult(pipeHandle, &overlapped, &cbRet, FALSE) &&
          cbRet != 0) {
        result = cbRet;
      } else {
        StringStorage errText;
        errText.format(_T("The Pipe's read function failed after GetOverlappedResult calling (pipe handle is %p)"), pipeHandle);
        StringStorage errMess;
        Environment::getErrStr(errText.getString(),
                               &errMess);
        throw IOException(errMess.getString());
      }
    } else {
      StringStorage errText;
      errText.format(_T("The Pipe's write function failed after ReadFile calling")
                     _T("(pipe handle is %p, total read %llu, try to read %u)"),
                     pipeHandle, m_totalRead, length);
      StringStorage errMess;
      Environment::getErrStr(errText.getString(), &errMess);
      throw IOException(errMess.getString());
    }
  } // else operation already successful has been completed

  if (result == 0) {
    throw IOException(_T("Unknown pipe error"));
  }
  m_totalRead += result;
  return result;
}

void Pipe::checkPipeHandle(HANDLE pipeHandle)
{
  if (pipeHandle == INVALID_HANDLE_VALUE) {
    throw IOException(_T("Invalid pipe handle"));
  }
}
