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

#include "RectSerializer.h"
#include <tchar.h>
#include <stdio.h>
#include "util/Exception.h"

void RectSerializer::toString(const Rect *rect, StringStorage *strOut)
{
  strOut->format(_T("%dx%d+%d+%d"), rect->getWidth(),
                                    rect->getHeight(),
                                    rect->left,
                                    rect->top);
}

Rect RectSerializer::toRect(const StringStorage *strIn)
{
  int width, height, x, y;
  TCHAR c;
  if (_stscanf(strIn->getString(),
               _T("%dx%d+%d+%d%c"), &width, &height, &x, &y, &c) != 4 ||
      width < 0 || height < 0) {
    StringStorage errMess;
    errMess.format(_T("Invalid string format to convert it to a rectangle")
                   _T(" (%s)."), strIn->getString());
    throw Exception(errMess.getString());
  }
  return Rect(x, y, x + width, y + height);
}
