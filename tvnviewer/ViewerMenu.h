// Copyright (C) 2011,2012 GlavSoft LLC.
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

#ifndef _VIEWER_MENU_H_
#define _VIEWER_MENU_H_

#include "gui/Menu.h"

class ViewerMenu: public Menu
{
public:
  void loadMenu();

protected:
  int m_startIndex;

  static const int SEPARATOR = 0;
  static const int MENU_ITEM = 1;

  typedef struct {
    int menu_id;
    int menu_param;
  } menu_item;

private:
  void makeSeparator();
  void menuItem(int id);
  void findStartIndex();
};

#endif
