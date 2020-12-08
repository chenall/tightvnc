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

#include "PasswordControl.h"

#include "util/VncPassCrypt.h"
#include "util/StringTable.h"
#include "util/AnsiStringStorage.h"

#include "ChangePasswordDialog.h"

#include "tvnserver/resource.h"

PasswordControl::PasswordControl(Control *changeButton, Control *unsetButton)
: m_enabled(true), m_changeButton(changeButton), m_unsetButton(unsetButton)
{
  updateControlsState();
}

PasswordControl::~PasswordControl()
{
  releaseCryptedPassword();
}

bool PasswordControl::hasPassword() const
{
  return m_cryptedPassword.size() != 0;
}

void PasswordControl::setEnabled(bool enabled)
{
  m_enabled = enabled;

  updateControlsState();
}

void PasswordControl::unsetPassword(bool promtUser, HWND parentWindow)
{
  if (promtUser) {
    if (MessageBox(parentWindow,
      StringTable::getString(IDS_UNSET_PASSWORD_PROMT),
      StringTable::getString(IDS_MBC_TVNCONTROL), MB_YESNO | MB_ICONQUESTION) == IDNO) {
      return;
    }
  }

  releaseCryptedPassword();

  updateControlsState();
}

void PasswordControl::setPassword(const TCHAR *plainText)
{
  char plainTextInANSI[9];
  memset(plainTextInANSI, 0, sizeof(plainTextInANSI));
  AnsiStringStorage ansiPlainTextStorage(&StringStorage(plainText));
  memcpy(plainTextInANSI, ansiPlainTextStorage.getString(),
         min(ansiPlainTextStorage.getLength(), sizeof(plainTextInANSI)));

  UINT8 cryptedPassword[8];
  memset(cryptedPassword, 0, 8);

  VncPassCrypt::getEncryptedPass(cryptedPassword, (const UINT8 *)plainTextInANSI);

  setCryptedPassword((char *)cryptedPassword);

  updateControlsState();
}

void PasswordControl::setCryptedPassword(const char *cryptedPass)
{
  releaseCryptedPassword();

  m_cryptedPassword.resize(8);
  memcpy(&m_cryptedPassword.front(), cryptedPass, m_cryptedPassword.size());

  updateControlsState();
}

const char *PasswordControl::getCryptedPassword() const
{
  if (m_cryptedPassword.empty()) {
    return 0;
  }

  return &m_cryptedPassword.front();
}

bool PasswordControl::showChangePasswordModalDialog(Control *parent)
{
  ChangePasswordDialog changePasswordDialog(parent, !hasPassword());

  if (changePasswordDialog.showModal() != IDOK) {
    return false;
  }

  setPassword(changePasswordDialog.getPasswordInPlainText());

  return true;
}

void PasswordControl::updateControlsState()
{
  if (m_changeButton != 0) {
    if (hasPassword()) {
      m_changeButton->setText(StringTable::getString(IDS_CHANGE_PASSWORD_CAPTION));
    } else {
      m_changeButton->setText(StringTable::getString(IDS_SET_PASSWORD_CAPTION));
    }
    m_changeButton->setEnabled(m_enabled);
  }
  if (m_unsetButton != 0) {
    m_unsetButton->setEnabled(m_enabled && hasPassword());
  }
}

void PasswordControl::releaseCryptedPassword()
{
  m_cryptedPassword.resize(0);
}
