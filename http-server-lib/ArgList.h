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

#ifndef _HTTP_ARGUMENT_LIST_H_
#define _HTTP_ARGUMENT_LIST_H_

#include <map>
#include <string>

using namespace std;

class ArgList
{
public:
  ArgList(const char *argString);
  virtual ~ArgList();

  // Return arguments count.
  size_t getCount() const;

  // Returns key name with specified index or NULL if index out of range.
  // It can be used for enumerating keys.
  const char *getKey(size_t index);

  // Returns value of argument with specified key or NULL if no such
  // argument.
  const char *getValue(const char *key);

protected:
  void splitPair(const char *par, char delimitter, char *key, char *value);

  void htmlDecode(char *value) const;

protected:
  map<string, char *> m_args;
};

#endif
