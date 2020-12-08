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

#ifndef __ANSISTRINGSTORAGE_H__
#define __ANSISTRINGSTORAGE_H__

#include "StringStorage.h"

class AnsiStringStorage
{
public:
  AnsiStringStorage();
  AnsiStringStorage(const char *string);
  AnsiStringStorage(const StringStorage *string);
  AnsiStringStorage(const AnsiStringStorage &stringBuffer);
  ~AnsiStringStorage();

  // Fills internal buffer by the string.
  virtual void setString(const char *string);

  // Returns pointer to the first symbol.
  const char *getString() const;

    // Returns length of string (in characters, not including terminating null character).
  size_t getLength() const;

  // Returns size of string in bytes, including terminating null character.
  size_t getSize() const;

  // Checks if string is empty.
  // @return true if string is empty.
  bool isEmpty() const;

  void fromStringStorage(const StringStorage *src);
  void toStringStorage(StringStorage *dst) const;

  void format(const char *format, ...);

  void appendString(const char *string);

private:
  typedef std::vector<char> BufferType;
  BufferType m_buffer;
};

#endif // __ANSISTRINGSTORAGE_H__
