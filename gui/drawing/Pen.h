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

#ifndef _PEN_H_
#define _PEN_H_

#include "util/CommonHeader.h"

class Pen
{
public:
  const static int SOLID_TYPE = PS_SOLID;
  const static int DASH_TYPE = PS_DASH;
  const static int NULL_TYPE = PS_NULL;

public:
  Pen(int type, int width, COLORREF color);
  virtual ~Pen();

protected:
  HPEN m_pen;

  friend class Graphics;
};

#endif
