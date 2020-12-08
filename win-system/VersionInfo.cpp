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

#include "VersionInfo.h"
#include "vector"

VersionInfo::VersionInfo(const TCHAR *pathToFile)
{
  DWORD handle = 0;
  DWORD verInfoSize = GetFileVersionInfoSize(pathToFile, &handle);

  if (verInfoSize == 0) {
    throw SystemException();
  }

  std::vector<char> charBuff(verInfoSize);
  char *verInfo = &charBuff.front();

  if (GetFileVersionInfo(pathToFile, handle, verInfoSize, verInfo) == 0) {
    throw SystemException();
  }

  UINT fixedInfoSize = 0;

  VS_FIXEDFILEINFO *fixedInfo = 0;

  if (VerQueryValue(verInfo, _T("\\"), (void **)&fixedInfo, &fixedInfoSize) == 0) {
    throw SystemException();
  }

  unsigned short a = ((unsigned short *)&fixedInfo->dwProductVersionMS)[1];
  unsigned short b = ((unsigned short *)&fixedInfo->dwProductVersionMS)[0];
  unsigned short c = ((unsigned short *)&fixedInfo->dwProductVersionLS)[1];
  unsigned short d = ((unsigned short *)&fixedInfo->dwProductVersionLS)[0];

  // Convert special numbers used for beta versions (third number > 100).
  // 2.0.105.0 should be considered a beta version and convert to 2.1beta5,
  // 1.99.101 converts to 2.0beta1, 1.99.101.23 converts to 2.0beta1.23 etc.
  TCHAR *secondDelimiter = _T(".");
  if (c > 100) {
    secondDelimiter = _T("beta");
    c -= 100;
    if (++b >= 100) {
      b = 0;
      a++;
    }
  }

  if (c == 0 && d == 0) {
    m_productVersionString.format(_T("%d.%d"), (int)a, (int)b);
  } else if (d == 0) {
    m_productVersionString.format(_T("%d.%d%s%d"), (int)a, (int)b,
                                  secondDelimiter, (int)c);
  } else {
    m_productVersionString.format(_T("%d.%d%s%d.%d"), (int)a, (int)b,
                                  secondDelimiter, (int)c, (int)d);
  }
}

const TCHAR *VersionInfo::getProductVersionString() const
{
  return m_productVersionString.getString();
}
