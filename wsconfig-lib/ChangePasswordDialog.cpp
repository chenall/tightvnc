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

#include "ChangePasswordDialog.h"
#include "tvnserver/resource.h"

#include "server-config-lib/ServerConfig.h"

#include "util/VncPassCrypt.h"

ChangePasswordDialog::ChangePasswordDialog(Control *parent, bool isNewPassword)
: BaseDialog(IDD_CHANGE_PASSWORD), m_allowEmptyPassword(false), m_newPassword(isNewPassword)
{
  setParent(parent);

  m_passwordEmptyTooltip.setText(StringTable::getString(IDS_PASSWORD_IS_EMPTY));
  m_passwordEmptyTooltip.setTitle(StringTable::getString(IDS_MBC_TVNCONTROL));

  m_passwordsNotMatchTooltip.setText(StringTable::getString(IDS_PASSWORDS_NOT_MATCH));
  m_passwordsNotMatchTooltip.setTitle(StringTable::getString(IDS_MBC_TVNCONTROL));
}

ChangePasswordDialog::~ChangePasswordDialog()
{
}

const TCHAR *ChangePasswordDialog::getPasswordInPlainText() const
{
  return m_passwordText.getString();
}

BOOL ChangePasswordDialog::onInitDialog()
{
  initControls();
  if (m_newPassword) {
    m_ctrlThis.setText(StringTable::getString(IDS_NEW_PASSWORD));
  } else {
    m_ctrlThis.setText(StringTable::getString(IDS_CHANGE_PASSWORD));
  }
  return TRUE;
}

BOOL ChangePasswordDialog::onCommand(UINT cID, UINT nID)
{
  if (nID == BN_CLICKED) {
    switch (cID) {
    case IDOK:
      onOkButtonClick();
      break;
    case IDCANCEL:
      onCancelButtonClick();
      break;
    }
  }
  return TRUE;
}

void ChangePasswordDialog::onOkButtonClick()
{
  StringStorage password1;
  StringStorage password2;
  m_password1.getText(&password1);
  m_password2.getText(&password2);

  if (password1.isEmpty() && !m_allowEmptyPassword) {
    m_password1.showBalloonTip(&m_passwordEmptyTooltip);
    m_password1.setFocus();
    return ;
  }
  if (!password1.isEqualTo(&password2)) {
    m_password2.showBalloonTip(&m_passwordsNotMatchTooltip);
    m_password2.setFocus();
    return ;
  }

  m_passwordText.setString(password1.getString());

  kill(IDOK);
}

void ChangePasswordDialog::onCancelButtonClick()
{
  kill(IDCANCEL);
}

void ChangePasswordDialog::initControls()
{
  HWND hwnd = m_ctrlThis.getWindow();
  m_password1.setWindow(GetDlgItem(hwnd, IDC_PASSWORD));
  m_password2.setWindow(GetDlgItem(hwnd, IDC_PASSWORD2));

  m_password1.setTextLengthLimit(VncPassCrypt::VNC_PASSWORD_SIZE);
  m_password2.setTextLengthLimit(VncPassCrypt::VNC_PASSWORD_SIZE);
}
