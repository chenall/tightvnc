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

#ifndef __CLIENTINPUTEVENTLISTENER_H__
#define __CLIENTINPUTEVENTLISTENER_H__

#include "util/inttypes.h"

class ClientInputEventListener
{
public:
  virtual void onKeyboardEvent(UINT32 keySym, bool down) = 0;
  virtual void onMouseEvent(UINT16 x, UINT16 y, UINT8 buttonMask) = 0;
};

#endif // __CLIENTINPUTEVENTLISTENER_H__
