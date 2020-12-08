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

#include "util/winhdr.h"
#include "tvnserver-app/NamingDefs.h"

#include "win-system/Registry.h"

#include "SetPasswordsDialog.h"

#include "tvnserver/resource.h"

SetPasswordsDialog::SetPasswordsDialog(bool initStateOfUseRfbAuth,
                                       bool initStateOfUseAdminAuth)
: BaseDialog(IDD_SET_PASSWORDS),
  m_dontChangeRfbAuth(!initStateOfUseRfbAuth),
  m_useRfbAuth(initStateOfUseRfbAuth),
  m_dontChangeAdmAuth(!initStateOfUseAdminAuth),
  m_protectControlInterface(initStateOfUseAdminAuth)
{
  m_passwordEmptyTooltip.setText(StringTable::getString(IDS_PASSWORD_IS_EMPTY));
  m_passwordEmptyTooltip.setTitle(StringTable::getString(IDS_MBC_TVNCONTROL));

  m_passwordsNotMatchTooltip.setText(StringTable::getString(IDS_PASSWORDS_NOT_MATCH));
  m_passwordsNotMatchTooltip.setTitle(StringTable::getString(IDS_MBC_TVNCONTROL));
}

SetPasswordsDialog::~SetPasswordsDialog()
{
}

void SetPasswordsDialog::getRfbPass(StringStorage *pass)
{
  *pass = m_rfbPass;
}

bool SetPasswordsDialog::getUseRfbPass()
{
  return m_useRfbAuth;
}

bool SetPasswordsDialog::getRfbPassForClear()
{
  return !m_useRfbAuth && !m_dontChangeRfbAuth;
}

bool SetPasswordsDialog::getUseAdmPass()
{
  return m_protectControlInterface;
}

bool SetPasswordsDialog::getAdmPassForClear()
{
  return !m_protectControlInterface && !m_dontChangeAdmAuth;
}

void SetPasswordsDialog::getAdmPass(StringStorage *pass)
{
  *pass = m_admPass;
}

void SetPasswordsDialog::initControls()
{
  HWND window = m_ctrlThis.getWindow();

  m_dontChangeRfbAuthSettingsRadio.setWindow(GetDlgItem(window,
    IDC_DONT_CHANGE_RFB_AUTH_SETTINGS_RADIO));
  m_dontUseRfbAuthRadio.setWindow(GetDlgItem(window, IDC_DONT_USE_RFB_AUTH_RADIO));
  m_useRfbAuthRadio.setWindow(GetDlgItem(window, IDC_USE_RFB_AUTH_RADIO));
  m_rfbPassEdit1.setWindow(GetDlgItem(window, IDC_ENTER_RFB_PASSWORD));
  m_rfbPassEdit2.setWindow(GetDlgItem(window, IDC_CONFIRM_RFB_PASSWORD));

  m_dontChangeAdminAuthSettingsRadio.setWindow(GetDlgItem(window,
    IDC_DONT_CHANGE_CONTROL_PROTECTION_SETTINGS_RADIO));
  m_dontUseAdminAuthRadio.setWindow(GetDlgItem(window, IDC_DONT_USE_CONTROL_PROTECTION_RADIO));
  m_useAdminAuthRadio.setWindow(GetDlgItem(window, IDC_PROTECT_CONTROL_INTERFACE_RADIO));
  m_admPassEdit1.setWindow(GetDlgItem(window, IDC_ENTER_ADMIN_PASSWORD));
  m_admPassEdit2.setWindow(GetDlgItem(window, IDC_CONFIRM_ADMIN_PASSWORD));

  m_rfbPassEdit1.setTextLengthLimit(8);
  m_rfbPassEdit2.setTextLengthLimit(8);
  m_admPassEdit1.setTextLengthLimit(8);
  m_admPassEdit2.setTextLengthLimit(8);
}

BOOL SetPasswordsDialog::onInitDialog()
{
  initControls();

  if (m_useRfbAuth) {
    m_useRfbAuthRadio.check(true);
  } else if (m_dontChangeRfbAuth) {
    m_dontChangeRfbAuthSettingsRadio.check(true);
  }
  if (m_protectControlInterface) {
    m_useAdminAuthRadio.check(true);
  } else if (m_dontChangeAdmAuth) {
    m_dontChangeAdminAuthSettingsRadio.check(true);
  }

  updateEditControls();

  return FALSE;
}

BOOL SetPasswordsDialog::onNotify(UINT controlID, LPARAM data)
{
  return FALSE;
}

BOOL SetPasswordsDialog::onCommand(UINT controlID, UINT notificationID)
{
  if (notificationID == BN_CLICKED) {
    readRadio();
    updateEditControls();
  }
  if (controlID == IDOK) {
    onOkButtonClick();
  }
  return FALSE;
}

BOOL SetPasswordsDialog::onDestroy()
{
  return FALSE;
}

void SetPasswordsDialog::onOkButtonClick()
{
  StringStorage rfbPass1;
  StringStorage rfbPass2;
  StringStorage admPass1;
  StringStorage admPass2;

  m_rfbPassEdit1.getText(&rfbPass1);
  m_rfbPassEdit2.getText(&rfbPass2);
  m_admPassEdit1.getText(&admPass1);
  m_admPassEdit2.getText(&admPass2);

  if (m_useRfbAuth) {
    if (rfbPass1.isEmpty()) {
      m_rfbPassEdit1.showBalloonTip(&m_passwordEmptyTooltip);
      m_rfbPassEdit1.setFocus();
      return;
    }
    if (!rfbPass1.isEqualTo(&rfbPass2)) {
      m_rfbPassEdit2.showBalloonTip(&m_passwordsNotMatchTooltip);
      m_rfbPassEdit2.setFocus();
      return;
    }
    m_rfbPass.setString(rfbPass1.getString());
  }
  if (m_protectControlInterface) {
    if (admPass1.isEmpty()) {
      m_admPassEdit1.showBalloonTip(&m_passwordEmptyTooltip);
      m_admPassEdit1.setFocus();
      return;
    }
    if (!admPass1.isEqualTo(&admPass2)) {
      m_admPassEdit2.showBalloonTip(&m_passwordsNotMatchTooltip);
      m_admPassEdit2.setFocus();
      return;
    }
    m_admPass.setString(admPass1.getString());
  }
  kill(IDOK);
}

void SetPasswordsDialog::readRadio()
{
  m_dontChangeRfbAuth = m_dontChangeRfbAuthSettingsRadio.isChecked();
  m_useRfbAuth = m_useRfbAuthRadio.isChecked();
  m_dontChangeAdmAuth = m_dontChangeAdminAuthSettingsRadio.isChecked();
  m_protectControlInterface = m_useAdminAuthRadio.isChecked();
}

void SetPasswordsDialog::updateEditControls()
{
  m_rfbPassEdit1.setEnabled(m_useRfbAuth);
  m_rfbPassEdit2.setEnabled(m_useRfbAuth);
  m_admPassEdit1.setEnabled(m_protectControlInterface);
  m_admPassEdit2.setEnabled(m_protectControlInterface);
}
