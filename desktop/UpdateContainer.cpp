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

#include "UpdateContainer.h"

UpdateContainer::UpdateContainer()
{
  clear();
}

UpdateContainer::~UpdateContainer(void)
{
}

void UpdateContainer::clear()
{
  copiedRegion.clear();
  changedRegion.clear();
  videoRegion.clear();
  screenSizeChanged = false;
  cursorPosChanged = false;
  cursorShapeChanged = false;
  copySrc.clear();
  cursorPos.clear();
}

UpdateContainer& UpdateContainer::operator=(const UpdateContainer& src)
{
  copiedRegion        = src.copiedRegion;
  changedRegion       = src.changedRegion;
  videoRegion         = src.videoRegion;
  screenSizeChanged   = src.screenSizeChanged;
  cursorPosChanged    = src.cursorPosChanged;
  cursorShapeChanged  = src.cursorShapeChanged;
  copySrc             = src.copySrc;
  cursorPos           = src.cursorPos;

  return *this;
}

bool UpdateContainer::isEmpty() const
{
  return copiedRegion.isEmpty() &&
         changedRegion.isEmpty() &&
         videoRegion.isEmpty() &&
         !screenSizeChanged &&
         !cursorPosChanged &&
         !cursorShapeChanged;
}
