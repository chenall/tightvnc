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

#ifndef __INPUTBLOCKER_H__
#define __INPUTBLOCKER_H__

#include "util/DateTime.h"

class InputBlocker
{
public:
  InputBlocker();
  virtual ~InputBlocker();

  // This functions set/unset blocks on a local keyboard and mouse.
  virtual void setKeyboardBlocking(bool block) = 0;
  virtual void setMouseBlocking(bool block) = 0;
  // This functions set/unset blocks on a local keyboard and mouse on the
  // timeInterval interval from a last software event generation.
  virtual void setSoftKeyboardBlocking(bool block, unsigned int timeInterval) = 0;
  virtual void setSoftMouseBlocking(bool block, unsigned int timeInterval) = 0;

  virtual DateTime getLastInputTime() const = 0;
  virtual void correctLastTime(DateTime newTime) = 0;

  virtual bool isRemoteInputAllowed() = 0;
};

#endif // __INPUTBLOCKER_H__
