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

#include "WinFilePath.h"

WinFilePath::WinFilePath()
: m_parentPathIsRoot(false)
{
}

WinFilePath::WinFilePath(const TCHAR *string)
: m_parentPathIsRoot(false)
{
  setString(string);
}

WinFilePath::WinFilePath(const WinFilePath &stringBuffer)
: m_parentPathIsRoot(false)
{
  setString(stringBuffer.getString());
}

WinFilePath::~WinFilePath()
{
}

bool WinFilePath::parentPathIsRoot()
{
  return m_parentPathIsRoot;
}

void WinFilePath::setString(const TCHAR *string)
{
  StringStorage str(string);
  if (!str.isEmpty()) {
    if (str.findLast('/') == 0) {
      m_parentPathIsRoot = true;
    }
    str.replaceChar(_T('/'), _T('\\'));
    if (str.beginsWith(_T('\\'))) {
      str.remove(0, 1);
    }
    if (str.endsWith(_T('\\'))) {
      str.truncate(1);
    }
  }
  StringStorage::setString(str.getString());
}
