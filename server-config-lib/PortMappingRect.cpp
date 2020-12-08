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

#include "PortMappingRect.h"
#include <tchar.h>
#include <stdio.h>

PortMappingRect::PortMappingRect(int l, int t, int r, int b)
 : Rect(l, t, r, b)
{
}

PortMappingRect::PortMappingRect()
{
}

PortMappingRect::~PortMappingRect()
{
}

void PortMappingRect::toString(StringStorage *string) const
{
  string->format(_T("%dx%d+%d+%d"), right - left, bottom - top, left, top);
}

bool PortMappingRect::parse(const TCHAR *string, PortMappingRect *out)
{
  int width, height, x, y;
  TCHAR c;
  if (_stscanf(string, _T("%dx%d+%d+%d%c"), &width, &height, &x, &y, &c) != 4) {
    return false;
  }
  if (width < 0 || height < 0) {
    return false;
  }
  if (out != NULL) {
    out->left = x;
    out->right = x + width;
    out->top = y;
    out->bottom = y + height;
  }
  return true;
}

bool PortMappingRect::tryParse(const TCHAR *string)
{
  return parse(string, NULL);
}
