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

#ifndef __WIN8CURSORSHAPE_H__
#define __WIN8CURSORSHAPE_H__

#include "rfb/CursorShape.h"

// This class is wrapper for CursorShape class to extend it behaviour for the Win8 desktop
// duplication model.
class Win8CursorShape
{
public:
  Win8CursorShape();
  virtual ~Win8CursorShape();

  const CursorShape *getCursorShape() const;

  CursorShape *getCursorShapeForWriting();

  // threadOwner is a number of latest thread which call this function.
  void setVisibility(bool value, int threadOwner);

  bool getIsVisible() const;

private:
  int m_threadOwner;

  bool m_isVisible;
  CursorShape m_emptyDimensionCursorShape;
  CursorShape m_cursorShape;
};

#endif // __WIN8CURSORSHAPE_H__
