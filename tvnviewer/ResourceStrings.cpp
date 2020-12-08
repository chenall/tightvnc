// Copyright (C) 2012 GlavSoft LLC.
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

#include "ResourceStrings.h"
#include "util/ResourceLoader.h"

ResourceStrings::ResourceStrings() 
: m_min(0), 
  m_max(0)
{
}

ResourceStrings::ResourceStrings(int min, int max) 
{
  m_min = min;
  m_max = max;
}

bool ResourceStrings::isValid(int res) 
{
  if (res >= m_min && res <= m_max) {
    return true;
  }
  return false;
}

StringStorage ResourceStrings::getStrRes(int strRes) 
{
  ResourceLoader *rLoader = ResourceLoader::getInstance();

  StringStorage str;
  rLoader->loadString(strRes, &str);
  return str.getString();
}

StringStorage ResourceStrings::getStrPureRes(int strRes) 
{
  StringStorage strTemp = getStrRes(strRes);
  size_t index = strTemp.findChar(_T('\t'));
  if (index != -1) {
    index /= sizeof(TCHAR);
    strTemp.truncate(1+index);
  }
  strTemp.removeChars(_T("&"), 1);
  return strTemp;
}
