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

#include "ResourceLoader.h"
#include "util/UnicodeStringStorage.h"

#include <crtdbg.h>

ResourceLoader::ResourceLoader(HINSTANCE appInst)
: m_appInstance(appInst)
{
}

ResourceLoader::~ResourceLoader()
{
}

HICON ResourceLoader::loadStandartIcon(const TCHAR *iconName)
{
  return LoadIcon(NULL, iconName);
}

HICON ResourceLoader::loadIcon(const TCHAR *iconName)
{
  return LoadIcon(m_appInstance, iconName);
}

bool ResourceLoader::loadString(UINT id, StringStorage *string)
{
  _ASSERT(string != 0);
  string->setString(_T("(Undef)"));

  //
  // Format of string table:
  // Strings are stored in groups of 16 strings.
  // Group format is:
  // | length  |     string     | length  | string  | ...
  // | 2 bytes | len * 2 bytes  | 2 bytes | len * 2 | ...
  // Strings stored in the UTF16-encoding.
  // 


  // Id of string-group, based from 0.
  int resId = (id / 16) + 1;
  HRSRC resHnd = FindResource(m_appInstance,
                              MAKEINTRESOURCE(resId),
                              RT_STRING);
  if (resHnd != 0) {
    HGLOBAL hGlobal = LoadResource(m_appInstance, resHnd);
    LPVOID lockRes = LockResource(hGlobal);

    WCHAR *lpStr = reinterpret_cast<WCHAR *>(lockRes);
    for (UINT i = 0; i < (id % 16); i++) {
      lpStr += 1 + static_cast<UINT16>(lpStr[0]);
    }

    UINT16 strLen = static_cast<UINT16>(lpStr[0]);

    std::vector<WCHAR> strBuff;
    strBuff.resize(strLen + 1);
    memcpy(&strBuff.front(), lpStr + 1, strLen * sizeof(WCHAR));
    strBuff[strLen] = L'\0';

    UnlockResource(lockRes);
    FreeResource(hGlobal);

    UnicodeStringStorage unicodeString;
    unicodeString.setString(&strBuff.front());
    unicodeString.toStringStorage(string);
  }
  return true;
}

HACCEL ResourceLoader::loadAccelerator(UINT id)
{
  return LoadAccelerators(m_appInstance,
                          MAKEINTRESOURCE(id)); 
}

HCURSOR ResourceLoader::loadStandardCursor(const TCHAR *id)
{
  return LoadCursor(0, id);
}

HCURSOR ResourceLoader::loadCursor(UINT id)
{
  return LoadCursor(m_appInstance, MAKEINTRESOURCE(id));
}
