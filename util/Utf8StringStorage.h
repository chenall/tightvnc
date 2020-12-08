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

#ifndef __UTF8STRINGSTORAGE_H__
#define __UTF8STRINGSTORAGE_H__

#include "StringStorage.h"

// Note: The Utf8StringStorage class contains utf8 symbols "as is". The class
// do not adds termination symbol(s) and do not remove it when
// it already presented.
class Utf8StringStorage
{
public:
  Utf8StringStorage();
  Utf8StringStorage(const std::vector<char> *utf8Buffer);
  Utf8StringStorage(const StringStorage *string);
  Utf8StringStorage(const Utf8StringStorage &string);
  ~Utf8StringStorage();

  // Sets internal buffer by an external utf8 string.
  void setString(const std::vector<char> *utf8Buffer);

  // Returns pointer to the string.
  const char *getString() const;

  // Returns size of string in bytes, including terminating null character.
  size_t getSize() const;

  void fromStringStorage(const StringStorage *src);
  void toStringStorage(StringStorage *dst);

private:
  std::vector<char> m_buffer;
};

#endif // __UTF8STRINGSTORAGE_H__
