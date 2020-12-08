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

#include "AnsiStringStorage.h"
#include "CommonHeader.h"
#include "util/Exception.h"
#include <crtdbg.h>

AnsiStringStorage::AnsiStringStorage()
{
  setString("");
}

AnsiStringStorage::AnsiStringStorage(const char *string)
{
  setString(string);
}

AnsiStringStorage::AnsiStringStorage(const StringStorage *string)
{
  fromStringStorage(string);
}

AnsiStringStorage::AnsiStringStorage(const AnsiStringStorage &stringBuffer)
{
  *this = stringBuffer;
}

AnsiStringStorage::~AnsiStringStorage()
{
}

void AnsiStringStorage::setString(const char *string)
{
  if (string == 0) {
    string = "";
  }
  size_t length = strlen(string);
  m_buffer.resize(length + 1);
  memcpy(&m_buffer.front(), string, getSize());
}

const char *AnsiStringStorage::getString() const
{
  return &m_buffer.front();
}

size_t AnsiStringStorage::getLength() const
{
  return m_buffer.size() - 1;
}

size_t AnsiStringStorage::getSize() const
{
  return m_buffer.size() * sizeof(char);
}

bool AnsiStringStorage::isEmpty() const
{
  return getLength() == 0;
}

void AnsiStringStorage::fromStringStorage(const StringStorage *src)
{
#ifndef _UNICODE
  setString(src->getString());
#else
  // WideCharToMultiByte returns result length including terminating null character	
  int symbolCount = WideCharToMultiByte(CP_ACP, 0, src->getString(), -1, 
                                        NULL, 0, NULL, NULL);

  // Allocate space for the requred size
  m_buffer.resize(symbolCount);

  // Convert to ansi
  int constrSize = (int)m_buffer.size();
  _ASSERT(constrSize == m_buffer.size());
  WideCharToMultiByte(CP_ACP, 0, src->getString(), -1,
                      &m_buffer.front(), symbolCount, NULL, NULL);
#endif
}

void AnsiStringStorage::toStringStorage(StringStorage *dst) const
{
#ifndef _UNICODE
  dst->setString(getString());
#else
  int symbolCount = (int)getSize();
  _ASSERT(symbolCount == getSize());
  std::vector<WCHAR> unicodeBuffer(symbolCount);
  int result = MultiByteToWideChar(CP_ACP, 0, &m_buffer.front(),
                                   symbolCount,
                                   &unicodeBuffer.front(),
                                   symbolCount);
  if (result == 0) {
    throw Exception(_T("Cannot convert from Ansi to StringStorage"));
  }

  dst->setString(&unicodeBuffer.front());
#endif
}

void AnsiStringStorage::format(const char *format, ...)
{
  va_list vl;

  va_start(vl, format);
  int count = _vscprintf(format, vl);
  va_end(vl);

  m_buffer.resize(count + 1);

  va_start(vl, format);
  vsprintf_s(&m_buffer.front(), count + 1, format, vl);
  va_end(vl);
}

void AnsiStringStorage::appendString(const char *string)
{
  if (string == 0) {
    return;
  }
  AnsiStringStorage src(string);

  BufferType::iterator to = m_buffer.begin() + getLength();
  BufferType::iterator fromFirst = src.m_buffer.begin();
  BufferType::iterator fromLast = src.m_buffer.begin() + src.getLength();

  m_buffer.insert(to, fromFirst, fromLast);
}