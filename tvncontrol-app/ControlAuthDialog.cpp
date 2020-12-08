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

#include "ControlAuthDialog.h"

#include "tvnserver/resource.h"
#include "server-config-lib/ServerConfig.h"

ControlAuthDialog::ControlAuthDialog()
: BaseDialog(IDD_CONTROL_AUTH)
{
}

ControlAuthDialog::~ControlAuthDialog()
{
}

const TCHAR *ControlAuthDialog::getPassword() const
{
  return m_password.getString();
}

BOOL ControlAuthDialog::onInitDialog()
{
  m_password.setString(_T(""));

  HWND hwnd = m_ctrlThis.getWindow();
  m_passwordTextBox.setWindow(GetDlgItem(hwnd, IDC_PASSWORD_EDIT));
  m_passwordTextBox.setTextLengthLimit(ServerConfig::VNC_PASSWORD_SIZE);

  SetForegroundWindow(hwnd);
  m_passwordTextBox.setFocus();

  return TRUE;
}

BOOL ControlAuthDialog::onNotify(UINT controlID, LPARAM data)
{
  return TRUE;
}

BOOL ControlAuthDialog::onCommand(UINT controlID, UINT notificationID)
{
  switch (controlID) {
  case IDOK:
    m_passwordTextBox.getText(&m_password);
    kill(controlID);
    break;
  case IDCANCEL:
    kill(controlID);
    break;
  }
  return TRUE;
}

BOOL ControlAuthDialog::onDestroy()
{
  return TRUE;
}
