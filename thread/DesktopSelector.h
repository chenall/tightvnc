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

#ifndef __DESKTOPSELECTOR_H__
#define __DESKTOPSELECTOR_H__

#include "util/CommonHeader.h"

class DesktopSelector
{
public:
  // This funtion gets a handle to a desktop that receive user inputs.
  // @return If success the function returns a handle to the desktop.
  // On fail the function returns zero.
  // @remark After use the returned handle must be called the
  // DesktopSelector::closeDesktop() function.
  static HDESK getInputDesktop();

  // This funtion gets a handle to a named desktop by the name.
  // @param: name - pointer to a StringStorage object that contain a valid
  // desktop name such as "Winlogon".
  // @return if success the function returns a handle to the desktop
  // that receives user inputs. On fail the function returns zero.
  // @remark After use the returned handle must be called the
  // DesktopSelector::closeDesktop() function.
  static HDESK getDesktop(const StringStorage *name);

  // This function closes a handle to a desktop.
  // @param hdesk is a handle to a desktop that will be closed.
  // @return If success the function return true else false.
  static bool closeDesktop(HDESK hdesk);

  // This function sets a desktop to a thread from that it was called.
  // @param newDesktop - is a handle to a desktop.
  // @return If success the function return true else false.
  static bool setDesktopToCurrentThread(HDESK newDesktop);

  // This function select a desktop that assigned by name or not to a current
  // thread from that it was called.
  // @param: name - pointer to a StringStorage object that contain a valid
  // desktop name such as "Winlogon" that will be assigned to the thread. If
  // name is null the input desktop will be assigned.
  // @return If success the function return true else false.
  static bool selectDesktop(const StringStorage *name = 0);

  // @param desktopName - is a pointer to a StringStorage object that will be
  // used to store the desktop name. If function has failed then the desktopName
  // object will not change. If the function succeeds, the name of a current
  // input desktop stores in the desktopName object.
  // @return If success the function return true else false.
  static bool getCurrentDesktopName(StringStorage *desktopName);

  // @param desktopName - is a pointer to a StringStorage object that will be
  // used to store the desktop name. If function has failed then the desktopName
  // object will not change. If the function succeeds, the desktop name of the
  // current thread stores in the desktopName object.
  // @return If success the function return true else false.
  static bool getThreadDesktopName(StringStorage *desktopName);

private:
  // This function gets desktop name by a handle to a desktop.
  // @param desktopName - is a pointer to a StringStorage object that will be
  // used to store the desktop name. If function has failed then the desktopName
  // object will not change. If the function succeeds, the desktop name
  // stores in the desktopName object.
  // @return If success the function return true else false.
  static bool getDesktopName(HDESK desktop, StringStorage *desktopName);
};

#endif //__DESKTOPSELECTOR_H__
