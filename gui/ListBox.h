// Copyright (C) 2008,2009,2010,2011,2012 GlavSoft LLC.
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

#ifndef _LIST_BOX_H_
#define _LIST_BOX_H_

#include "Control.h"
#include "util/StringStorage.h"

class ListBox : public Control
{
public:
  ListBox();
  ~ListBox();
public:
  void addString(const TCHAR *str);
  void addString(const TCHAR *str, void *tag);
  void getItemText(int index, StringStorage *storage);
  void setItemText(int index, const TCHAR *str);
  void insertString(int index, const TCHAR *str);
  void insertString(int index, const TCHAR *str, LPARAM data);
  void appendString(const TCHAR *str, LPARAM data);
  void setItemData(int index, LPARAM data);
  void removeString(int index);
  int getSelectedIndex();
  int getTopIndex();
  void setTopIndex(int index);
  void setSelectedIndex(int index);
  LPARAM getItemData(int index);
  int getCount();
  void clear();
};

#endif
