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

#include "CtrlAltDelSimulator.h"
#include "thread/DesktopSelector.h"

CtrlAltDelSimulator::CtrlAltDelSimulator()
{
  resume();
}

CtrlAltDelSimulator::~CtrlAltDelSimulator()
{
}

void CtrlAltDelSimulator::execute()
{
  // Switch thread desktop to "Winlogon".
  if (DesktopSelector::selectDesktop(&StringStorage(_T("Winlogon")))) {
    HWND hwndCtrlAltDel = FindWindow(_T("SAS window class"), _T("SAS window"));
    if (hwndCtrlAltDel == NULL) {
      hwndCtrlAltDel = HWND_BROADCAST;
    }
    PostMessage(hwndCtrlAltDel, WM_HOTKEY, 0, MAKELONG(MOD_ALT | MOD_CONTROL, VK_DELETE));
  }
  // Do not restore previous desktop.
}
