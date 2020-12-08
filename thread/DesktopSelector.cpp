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

#include "DesktopSelector.h"
#include <vector>

HDESK DesktopSelector::getInputDesktop()
{
  return OpenInputDesktop(0, TRUE,
                          DESKTOP_CREATEMENU |
                          DESKTOP_CREATEWINDOW |
                          DESKTOP_ENUMERATE |
                          DESKTOP_HOOKCONTROL |
                          DESKTOP_WRITEOBJECTS |
                          DESKTOP_READOBJECTS |
                          DESKTOP_SWITCHDESKTOP |
                          GENERIC_WRITE);
}

HDESK DesktopSelector::getDesktop(const StringStorage *name)
{
  return OpenDesktop(name->getString(), 0, TRUE,
                     DESKTOP_CREATEMENU |
                     DESKTOP_CREATEWINDOW |
                     DESKTOP_ENUMERATE |
                     DESKTOP_HOOKCONTROL |
                     DESKTOP_WRITEOBJECTS |
                     DESKTOP_READOBJECTS |
                     DESKTOP_SWITCHDESKTOP |
                     GENERIC_WRITE);
}

bool DesktopSelector::closeDesktop(HDESK hdesk)
{
  return CloseDesktop(hdesk) != 0;
}

bool DesktopSelector::setDesktopToCurrentThread(HDESK newDesktop)
{
  return SetThreadDesktop(newDesktop) != 0;
}

bool DesktopSelector::selectDesktop(const StringStorage *name)
{
  HDESK desktop;
  if (name) {
    desktop = getDesktop(name);
  } else {
    desktop = getInputDesktop();
  }

  bool result = setDesktopToCurrentThread(desktop) != 0;
  closeDesktop(desktop);

  return result;
}

bool DesktopSelector::getDesktopName(HDESK desktop, StringStorage *desktopName)
{
  desktopName->setString(_T(""));

  DWORD nameLength = 0;
  // Do not check returned value because the function will return FALSE always.
  GetUserObjectInformation(desktop, UOI_NAME, 0, 0, &nameLength);

  if (nameLength != 0) {
    std::vector<TCHAR> name(nameLength);
    bool result = !!GetUserObjectInformation(desktop,
                                             UOI_NAME,
                                             &name[0],
                                             nameLength,
                                             0);
    if (result) {
      desktopName->setString(&name[0]);
      return true;
    }
  }
  return false;
}

bool DesktopSelector::getCurrentDesktopName(StringStorage *desktopName)
{
  HDESK inputDesktop = getInputDesktop();
  bool result = getDesktopName(inputDesktop, desktopName);
  closeDesktop(inputDesktop);
  return result;
}

bool DesktopSelector::getThreadDesktopName(StringStorage *desktopName)
{
  return getDesktopName(GetThreadDesktop(GetCurrentThreadId()), desktopName);
}
