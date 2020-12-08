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

#include "UnicodeStringStorage.h"
#include "CommonHeader.h"
#include "util/Exception.h"
#include <crtdbg.h>

UnicodeStringStorage::UnicodeStringStorage()
{
  setString(L"");
}

UnicodeStringStorage::UnicodeStringStorage(const WCHAR *string)
{
  setString(string);
}

UnicodeStringStorage::UnicodeStringStorage(const StringStorage *string)
{
  fromStringStorage(string);
}

UnicodeStringStorage::UnicodeStringStorage(const UnicodeStringStorage &stringBuffer)
{
  *this = stringBuffer;
}

UnicodeStringStorage::~UnicodeStringStorage()
{
}

void UnicodeStringStorage::setString(const WCHAR *string)
{
  if (string == 0) {
    string = L"";
  }

  size_t length = wcslen(string);
  m_buffer.resize(length + 1);
  memcpy(&m_buffer.front(), string, getSize());
}

const WCHAR *UnicodeStringStorage::getString() const
{
  return &m_buffer.front();
}

size_t UnicodeStringStorage::getLength() const
{
  return m_buffer.size() - 1;
}

size_t UnicodeStringStorage::getSize() const
{
  return m_buffer.size() * sizeof(WCHAR);
}

bool UnicodeStringStorage::isEmpty() const
{
  return getLength() == 0;
}

void UnicodeStringStorage::fromStringStorage(const StringStorage *src)
{
#ifndef _UNICODE
  int constrCharCount = (int)(src->getLength() + 1);
  _ASSERT(constrCharCount == src->getLength() + 1);
  m_buffer.resize(constrCharCount);
  MultiByteToWideChar(CP_ACP, 0, src->getString(),
                      constrCharCount, // Size in bytes
                      &m_buffer.front(),
                      static_cast<int>(m_buffer.size()) // Size in WCHAR
                      );
#else
  setString(src->getString());
#endif
}

void UnicodeStringStorage::toStringStorage(StringStorage *dst)
{
  dst->setString(_T(""));
#ifndef _UNICODE
  int symbolCount = (int)m_buffer.size();
  _ASSERT(symbolCount == m_buffer.size());

  int dstBuffSize = WideCharToMultiByte(CP_ACP, 0, getString(), symbolCount, 0, 0, 0, 0);
  if (dstBuffSize <= 0) {
    return;
  }
  // Allocate space for the requred size
  std::vector<char> ansiBuffer(dstBuffSize);

  // Convert to ansi
  if (WideCharToMultiByte(CP_ACP, 0, getString(), symbolCount,
                          &ansiBuffer.front(), dstBuffSize, 0, 0) != 0) {
    dst->setString(&ansiBuffer.front());
  }
#else
  dst->setString(getString());
#endif
}
