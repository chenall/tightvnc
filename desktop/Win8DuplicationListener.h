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

#ifndef __WIN8DUPLICATIONLISTENER_H__
#define __WIN8DUPLICATIONLISTENER_H__

#include "region/Region.h"

class Win8DuplicationListener
{
public:
  // changedRegion in target FrameBuffer coordinates.
  virtual void onFrameBufferUpdate(const Region *changedRegion) = 0;
  // dstRect, srcX, srcY in target FrameBuffer coordinates.
  virtual void onCopyRect(const Rect *dstRect, int srcX, int srcY) = 0;

  virtual void onCursorPositionChanged(int x, int y) = 0;
  virtual void onCursorShapeChanged() = 0;

  // Calls when an error occurred which can be recover by object recreating.
  virtual void onRecoverableError(const TCHAR *reason) = 0;
  // Calls when an error occurred which can't be recover by object recreating.
  // Must be selected other way to duplicate desktop.
  virtual void onCriticalError(const TCHAR *reason) = 0;
};

#endif // __WIN8DUPLICATIONLISTENER_H__
