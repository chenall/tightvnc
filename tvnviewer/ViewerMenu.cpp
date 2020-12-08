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

#include "ViewerMenu.h"
#include "resource.h"

#include "util/ResourceLoader.h"

void ViewerMenu::makeSeparator()
{
  insertSeparator(m_startIndex); 
  m_startIndex++;
}

void ViewerMenu::menuItem(int id)
{
  StringStorage helpString;
  ResourceLoader *rLoader = ResourceLoader::getInstance();
  rLoader->loadString(id, &helpString);
  insertMenuItem(m_startIndex, helpString.getString(), id);
  m_startIndex++;
}

void ViewerMenu::findStartIndex()
{
  if ((m_startIndex = findMenuItem(SC_CLOSE)) == -1) {
    m_startIndex= getMenuItemCount();
  } else if (m_startIndex > 0) {
    --m_startIndex;
  }
}

void ViewerMenu::loadMenu()
{
  menu_item menu_items[] = {
    {SEPARATOR, 0},
    {MENU_ITEM, IDS_TB_NEWCONNECTION},
    {MENU_ITEM, IDS_TB_SAVESESSION},
    {MENU_ITEM, IDS_TB_CONNOPTIONS},
    {MENU_ITEM, IDS_TB_CONNINFO},
    {SEPARATOR, 0},
    {MENU_ITEM, IDS_TB_PAUSE},
    {MENU_ITEM, IDS_TB_REFRESH},
    {SEPARATOR, 0},
    {MENU_ITEM, IDS_TB_CTRLALTDEL},
    {MENU_ITEM, IDS_TB_CTRLESC},
    {MENU_ITEM, IDS_TB_CTRL},
    {MENU_ITEM, IDS_TB_ALT},
    {SEPARATOR, 0},
    {MENU_ITEM, IDS_TB_TRANSFER},
    {SEPARATOR, 0},
    {MENU_ITEM, IDS_TB_TOOLBAR},
    {MENU_ITEM, IDS_TB_FULLSCREEN},
    {SEPARATOR, 0},
    {MENU_ITEM, IDS_TB_CONFIGURATION},
    {MENU_ITEM, IDS_ABOUT_VIEWER}
  };
  findStartIndex();

  for (size_t i = 0; i < sizeof(menu_items) / sizeof(menu_item); i++) {
    if (menu_items[i].menu_id == SEPARATOR) {
      makeSeparator();
    }
    if (menu_items[i].menu_id == MENU_ITEM) {
      menuItem(menu_items[i].menu_param);
    }
  }
}

