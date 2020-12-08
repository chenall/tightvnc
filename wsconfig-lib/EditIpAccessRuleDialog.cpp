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

#include "tvnserver/resource.h"
#include "EditIpAccessRuleDialog.h"
#include "util/StringParser.h"
#include "util/AnsiStringStorage.h"

EditIpAccessRuleDialog::EditIpAccessRuleDialog()
: BaseDialog(IDD_EDIT_IP_ACESS_CONTROL), m_data(NULL), m_isOpenedForEdit(false)
{
  m_warningBalloonTip.setIconType(TTI_WARNING);
  m_warningBalloonTip.setText(StringTable::getString(IDS_IP_ADDRESS_HINT));
  m_warningBalloonTip.setTitle(StringTable::getString(IDS_INVALID_IP_TITLE));

  m_lastIpLessThanFirstBT.setIconType(TTI_WARNING);
  m_lastIpLessThanFirstBT.setText(StringTable::getString(IDS_LAST_IP_MUST_BE_MORE_THAN_FIRST));
  m_lastIpLessThanFirstBT.setTitle(StringTable::getString(IDS_CAPTION_BAD_INPUT));
}

EditIpAccessRuleDialog::~EditIpAccessRuleDialog()
{
}

void EditIpAccessRuleDialog::setEditFlag(bool flagEnabled)
{
  m_isOpenedForEdit = flagEnabled;
}

BOOL EditIpAccessRuleDialog::onInitDialog()
{
  initControls();

  for (int i = 0; i < 3; i++) {
    m_access[i].check(false);
  }

  if (m_data == NULL) {
    return TRUE;
  }

  switch (m_data->getAction()) {
  case IpAccessRule::ACTION_TYPE_ALLOW:
    m_access[0].check(true);
    break;
  case IpAccessRule::ACTION_TYPE_DENY:
    m_access[1].check(true);
    break;
  case IpAccessRule::ACTION_TYPE_QUERY:
    m_access[2].check(true);
    break;
  }

  if (m_isOpenedForEdit) {
    StringStorage firstIp;
    StringStorage lastIp;

    m_data->getFirstIp(&firstIp);
    m_data->getLastIp(&lastIp);

    m_firstIp.setText(firstIp.getString());
    m_lastIp.setText(lastIp.getString());

    m_ctrlThis.setText(StringTable::getString(IDS_EDIT_IP_ACCESS_RULE_DIALOG_CAPTION));
  } else {
    m_ctrlThis.setText(StringTable::getString(IDS_NEW_IP_ACCESS_RULE_DIALOG_CAPTION));
  }

  return FALSE;
}

BOOL EditIpAccessRuleDialog::onCommand(UINT cID, UINT nID)
{
  if (nID == BN_CLICKED) {
    switch (cID) {
    case IDOK:
      onOkButtonClick();
      break;
    case IDCANCEL:
      onCancelButtonClick();
      break;
    case IDC_ALLOW:
      onAccessTypeRadioClick(0);
      break;
    case IDC_DENY:
     onAccessTypeRadioClick(1);
      break;
    case IDC_QUERY:
      onAccessTypeRadioClick(2);
      break;
    }
  }
  return TRUE;
}

void EditIpAccessRuleDialog::onOkButtonClick()
{
  if (!validateInput()) {
    return ;
  }
  if (m_data != NULL) {
    if (m_access[0].isChecked()) {
      m_data->setAction(IpAccessRule::ACTION_TYPE_ALLOW);
    } else if (m_access[1].isChecked()) {
      m_data->setAction(IpAccessRule::ACTION_TYPE_DENY);
    } else if (m_access[2].isChecked()) {
      m_data->setAction(IpAccessRule::ACTION_TYPE_QUERY);
    }

    StringStorage firstIp;
    StringStorage lastIp;

    m_firstIp.getText(&firstIp);
    m_lastIp.getText(&lastIp);

    m_data->setFirstIp(firstIp.getString());
    m_data->setLastIp(lastIp.getString());

  } // if
  kill(IDOK);
}

void EditIpAccessRuleDialog::onCancelButtonClick()
{
  kill(IDCANCEL);
}

void EditIpAccessRuleDialog::onAccessTypeRadioClick(int num)
{
  if (!m_access[num].isChecked()) {
    m_access[num].check(true);
    for (int i = 0; i < 3; i++) {
      if (i != num) {
        m_access[i].check(false);
      } // if
    } // for
  } // if
} // void

void EditIpAccessRuleDialog::initControls()
{
  HWND hwnd = m_ctrlThis.getWindow();

  m_firstIp.setWindow(GetDlgItem(hwnd, IDC_FIRST_IP));
  m_lastIp.setWindow(GetDlgItem(hwnd, IDC_LAST_IP));

  m_access[0].setWindow(GetDlgItem(hwnd, IDC_ALLOW));
  m_access[1].setWindow(GetDlgItem(hwnd, IDC_DENY));
  m_access[2].setWindow(GetDlgItem(hwnd, IDC_QUERY));
}

bool EditIpAccessRuleDialog::validateInput()
{
  StringStorage firstIp;
  StringStorage lastIp;

  m_firstIp.getText(&firstIp);
  m_lastIp.getText(&lastIp);

  if (!IpAccessRule::isIpAddressStringValid(firstIp.getString())) {
    m_firstIp.setFocus();
    m_firstIp.showBalloonTip(&m_warningBalloonTip);
    return false;
  } // if

  if (lastIp.isEmpty()) {
    return true;
  }

  if (!IpAccessRule::isIpAddressStringValid(lastIp.getString())) {
    m_lastIp.setFocus();
    m_lastIp.showBalloonTip(&m_warningBalloonTip);
    return false;
  } // if

  AnsiStringStorage firstIpAnsi(&firstIp);
  AnsiStringStorage lastIpAnsi(&lastIp);

  unsigned long firstIpAddr = inet_addr(firstIpAnsi.getString());
  unsigned long lastIpAddr = inet_addr(lastIpAnsi.getString());

  if (IpAccessRule::compareIp(firstIpAddr, lastIpAddr) == 1) {
    m_lastIp.setFocus();
    m_lastIp.showBalloonTip(&m_lastIpLessThanFirstBT);
    return false;
  }

  return true;
}
