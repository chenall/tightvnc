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

#include "Win8CursorShape.h"

Win8CursorShape::Win8CursorShape()
: m_isVisible(false),
  m_threadOwner(-1)
{
}

Win8CursorShape::~Win8CursorShape()
{
}

const CursorShape *Win8CursorShape::getCursorShape() const
{
  if (getIsVisible()) {
    return &m_cursorShape;
  } else {
    return &m_emptyDimensionCursorShape;
  }
}

CursorShape *Win8CursorShape::getCursorShapeForWriting()
{
  return &m_cursorShape;
}

void Win8CursorShape::setVisibility(bool value, int threadOwner)
{
  // Set to false is allowed only for a thread owner. Else, a not owner thread can
  // set the false value after owner thread and then cusrsor shape will disappear.
  if (value || m_threadOwner == threadOwner) {
    m_isVisible = value;
    m_threadOwner = threadOwner;
  }
}

bool Win8CursorShape::getIsVisible() const
{
  return m_isVisible;
}
