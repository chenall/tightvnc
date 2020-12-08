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

#ifndef __POINT_H__
#define __POINT_H__

struct Point
{
  Point() : x(0), y(0) {}
  Point(int x_, int y_) : x(x_), y(y_) {}

  inline void clear() { x = 0; y = 0; }
  inline void setPoint(int x_, int y_) { x = x_; y = y_; }
  inline void move(int deltaX, int deltaY) { x += deltaX; y += deltaY; }
  bool isEqualTo(const Point *other) const { return x == other->x &&
                                                    y == other->y; }

  int x;
  int y;
};

#endif // __POINT_H__
