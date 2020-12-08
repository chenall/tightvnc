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

#ifndef __WINHANDLES_H__
#define __WINHANDLES_H__

#include "util/CommonHeader.h"

class WinHandles
{
public:
  // This function allows to assign a handle for another process.
  // @param hSource is the handle that will be assigned for another process
  // @param hTargetProc is the handle to another process returned by the
  // winapi OpenProcess() function.
  // @param If the neededToCloseSource argument set to true then after handle
  // been returned the source handle will be closed.
  // @param If the keepCloseRight argument set to true then after handle
  // been returned it can be closed by current process.
  // @return The function returns the handle that assigned for another process
  // @throws Exception on a fail.
  static HANDLE assignHandleFor(HANDLE hSource,
                                HANDLE hTargetProc,
                                bool neededToCloseSource,
                                bool keepCloseRight);

  // This function allows to assign a handle for another process.
  // @param hSource is the handle that will be assigned for another process
  // @param procId is a target process id.
  // @param If the neededToCloseSource argument set to true then after handle
  // been returned the source handle will be closed.
  // @param If the keepCloseRight argument set to true then after handle
  // been returned it can be closed by current process.
  // @return The function returns the handle that assigned for another process
  // @throws Exception on a fail.
  static HANDLE assignHandleFor(HANDLE hSource,
                                unsigned int procId,
                                bool neededToCloseSource,
                                bool keepCloseRight);

private:
  WinHandles();
};

#endif // __WINHANDLES_H__
