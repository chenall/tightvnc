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
#include "IpAccessControlDialog.h"
#include "ConfigDialog.h"
#include "CommonInputValidation.h"
#include "UIDataAccess.h"
#include "server-config-lib/Configurator.h"
#include "server-config-lib/IpAccessControl.h"
#include "util/CommonHeader.h"
#include "util/StringParser.h"
#include "util/AnsiStringStorage.h"

IpAccessControlDialog::IpAccessControlDialog()
: BaseDialog(IDD_CONFIG_ACCESS_CONTROL_PAGE), m_parentDialog(NULL)
{
}

IpAccessControlDialog::~IpAccessControlDialog()
{
}

void IpAccessControlDialog::setParentDialog(BaseDialog *dialog)
{
  m_parentDialog = dialog;
}

BOOL IpAccessControlDialog::onInitDialog()
{
  m_config = Configurator::getInstance()->getServerConfig();
  m_container = m_config->getAccessControl();
  m_editDialog.setParent(&m_ctrlThis);
  initControls();
  updateUI();
  updateButtonsState();
  onIpCheckUpdate();
  return TRUE;
}

BOOL IpAccessControlDialog::onCommand(UINT controlID, UINT notificationID)
{
  if (notificationID == BN_CLICKED) {
    switch (controlID) {
    case IDC_ADD_BUTTON:
      onAddButtonClick();
      break;
    case IDC_EDIT_BUTTON:
      onEditButtonClick();
      break;
    case IDC_REMOVE_BUTTON:
      onRemoveButtonClick();
      break;
    case IDC_MOVE_UP_BUTTON:
      onMoveUpButtonClick();
      break;
    case IDC_MOVE_DOWN_BUTTON:
      onMoveDownButtonClick();
      break;
    case IDC_ACCEPT:
      onAcceptRadioClick();
      break;
    case IDC_REFUSE:
      onRefuseRadioClick();
      break;
    case IDC_ALLOW_LOOPBACK_CONNECTIONS:
      onAllowLoopbackConnectionsClick();
      break;
    case IDC_ALLOW_ONLY_LOOPBACK_CONNECTIONS:
      onAllowOnlyLoopbackConnectionsClick();
      break;
    }
  } else if (notificationID == EN_UPDATE) {
    switch (controlID) {
    case IDC_IP_FOR_CHECK_EDIT:
      onIpCheckUpdate();
      break;
    case IDC_TIMEOUT:
      onQueryTimeoutUpdate();
      break;
    }
  }
  return TRUE;
}

BOOL IpAccessControlDialog::onNotify(UINT controlID, LPARAM data)
{
  if (controlID == IDC_IP_ACCESS_CONTROL_LIST) {
    NMHDR* pnmh = (NMHDR*)data;
    switch (pnmh->code) {
    case LVN_ITEMCHANGED:
      onListViewSelChange();
      break;
    case NM_DBLCLK:
      onListViewSelChangeDblClick();
      break;
    }
  }
  return TRUE;
}

bool IpAccessControlDialog::validateInput()
{
  if (!CommonInputValidation::validateUINT(
    &m_queryTimeout,
    StringTable::getString(IDS_INVALID_QUERY_TIMEOUT))) {
    return false;
  }

  unsigned int queryTimeout;

  UIDataAccess::queryValueAsUInt(&m_queryTimeout, &queryTimeout);

  if (queryTimeout < ServerConfig::MINIMAL_QUERY_TIMEOUT) {
    CommonInputValidation::notifyValidationError(
      &m_queryTimeout,
      StringTable::getString(IDS_QUERY_TIMEOUT_TOO_SMALL));
    return false;
  }

  return true;
}

void IpAccessControlDialog::updateUI()
{
  m_list.clear();
  for (size_t i = 0; i < m_container->size(); i++) {
    IpAccessRule *ip = m_container->at(i);
    m_list.addItem(m_list.getCount(), _T(""), (LPARAM)ip);
    _ASSERT((int)i == i);
    setListViewItemText((int)i, ip);
  }

  m_allowLoopbackConnections.check(m_config->isLoopbackConnectionsAllowed());
  m_onlyLoopbackConnections.check(m_config->isOnlyLoopbackConnectionsAllowed());
  if (m_config->isDefaultActionAccept()) {
    m_defaultActionAccept.check(true);
    m_defaultActionRefuse.check(false);
  } else {
    m_defaultActionAccept.check(false);
    m_defaultActionRefuse.check(true);
  }
  m_queryTimeout.setUnsignedInt(m_config->getQueryTimeout());

  updateCheckBoxesState();
}

void IpAccessControlDialog::apply()
{
  // Query timeout string storage
  StringStorage qtStringStorage;
  m_queryTimeout.getText(&qtStringStorage);

  int timeout = 0;
  StringParser::parseInt(qtStringStorage.getString(), &timeout);

  AutoLock al(m_config);

  m_config->allowLoopbackConnections(m_allowLoopbackConnections.isChecked());
  m_config->acceptOnlyLoopbackConnections(m_onlyLoopbackConnections.isChecked());
  m_config->setDefaultActionToAccept(m_defaultActionAccept.isChecked());
  m_config->setQueryTimeout(timeout);

  //
  // Put IP access rules container in correct order
  //

  IpAccessControl *ipRules = m_config->getAccessControl();
  ipRules->clear();
  for (int i = 0; i < m_list.getCount(); i++) {
    IpAccessRule *rule = (IpAccessRule *)m_list.getItemData(i);
    ipRules->push_back(rule);
  }
}

void IpAccessControlDialog::initControls()
{
  HWND hwnd = m_ctrlThis.getWindow();
  m_list.setWindow(GetDlgItem(hwnd, IDC_IP_ACCESS_CONTROL_LIST));
  m_addButton.setWindow(GetDlgItem(hwnd, IDC_ADD_BUTTON));
  m_editButton.setWindow(GetDlgItem(hwnd, IDC_EDIT_BUTTON));
  m_removeButton.setWindow(GetDlgItem(hwnd, IDC_REMOVE_BUTTON));
  m_moveUpButton.setWindow(GetDlgItem(hwnd, IDC_MOVE_UP_BUTTON));
  m_moveDownButton.setWindow(GetDlgItem(hwnd, IDC_MOVE_DOWN_BUTTON));
  m_defaultActionAccept.setWindow(GetDlgItem(hwnd, IDC_ACCEPT));
  m_defaultActionRefuse.setWindow(GetDlgItem(hwnd, IDC_REFUSE));
  m_queryTimeout.setWindow(GetDlgItem(hwnd, IDC_TIMEOUT));
  m_allowLoopbackConnections.setWindow(GetDlgItem(hwnd, IDC_ALLOW_LOOPBACK_CONNECTIONS));
  m_onlyLoopbackConnections.setWindow(GetDlgItem(hwnd, IDC_ALLOW_ONLY_LOOPBACK_CONNECTIONS));
  m_queryTimeoutSpin.setWindow(GetDlgItem(hwnd, IDC_QUERY_TIMEOUT_SPIN));
  m_ip.setWindow(GetDlgItem(hwnd, IDC_IP_FOR_CHECK_EDIT));
  m_ipCheckResult.setWindow(GetDlgItem(hwnd, IDC_IP_CHECK_RESULT_LABEL));

  m_list.addColumn(0, StringTable::getString(IDS_FIRST_IP_COLUMN), 100);
  m_list.addColumn(1, StringTable::getString(IDS_LAST_IP_COLUMN), 100);
  m_list.addColumn(2, StringTable::getString(IDS_ACTION_COLUMN), 80);

  m_list.allowMultiSelection(false);
  m_list.setFullRowSelectStyle(true);

  m_queryTimeoutSpin.setBuddy(&m_queryTimeout);
  m_queryTimeoutSpin.setAccel(0, 1);
  m_queryTimeoutSpin.setRange32(0, INT_MAX);
}

void IpAccessControlDialog::onAddButtonClick()
{
  IpAccessRule *ip = new IpAccessRule();
  m_editDialog.setIpAccessControl(ip);
  m_editDialog.setEditFlag(false);
  if (m_editDialog.showModal() == IDOK) {
    m_container->push_back(ip);
    m_list.addItem(m_list.getCount(), _T(""), (LPARAM)ip);
    setListViewItemText(m_list.getCount() - 1, ip);
    updateButtonsState();
    onIpCheckUpdate();
    ((ConfigDialog *)m_parentDialog)->updateApplyButtonState();
  } else {
    delete ip;
  }
  m_list.invalidate();
  m_ctrlThis.invalidate();
}

void IpAccessControlDialog::onEditButtonClick()
{
  // If nothing selected
  if (m_list.getSelectedIndex() == -1) {
    return ;
  }
  IpAccessRule *ip = (IpAccessRule *)m_list.getItemData(m_list.getSelectedIndex());
  m_editDialog.setIpAccessControl(ip);
  m_editDialog.setEditFlag(true);
  if (m_editDialog.showModal() == IDOK) {
    setListViewItemText(m_list.getSelectedIndex(), ip);
    updateButtonsState();
    onIpCheckUpdate();
    ((ConfigDialog *)m_parentDialog)->updateApplyButtonState();
  } else {
  }
  m_list.invalidate();
  m_ctrlThis.invalidate();
}

void IpAccessControlDialog::onRemoveButtonClick()
{
  int si = m_list.getSelectedIndex();
  // If nothing selected
  if (si == -1) {
    return ;
  }
  {
    AutoLock al(m_config);
    IpAccessRule *ip = (IpAccessRule *)m_list.getItemData(m_list.getSelectedIndex());
    for (IpAccessControl::iterator it = m_container->begin(); it != m_container->end(); it++) {
      IpAccessRule *ip2 = *it;
      if (ip == ip2) {
        m_container->erase(it);
        m_list.removeItem(si);
        updateButtonsState();
        onIpCheckUpdate();
        ((ConfigDialog *)m_parentDialog)->updateApplyButtonState();
        break;
      }
    }
    delete ip;
  }
  m_list.selectItem(si);
  if (m_list.getSelectedIndex() == -1) {
    m_list.selectItem(si - 1);
  } else if (m_list.getSelectedIndex() == -1) {
    m_list.selectItem(si + 1);
  }
}

void IpAccessControlDialog::onMoveUpButtonClick()
{
  int si = m_list.getSelectedIndex();
  // If nothing selected
  if ((si == -1) || (si == 0)) {
    return ;
  }

  IpAccessRule *ip = (IpAccessRule *)m_list.getItemData(si);
  IpAccessRule *ipPrev = (IpAccessRule *)m_list.getItemData(si - 1);

  setListViewItemText(si - 1, ip);
  setListViewItemText(si, ipPrev);
  m_list.selectItem(si - 1);
  onIpCheckUpdate();
  ((ConfigDialog *)m_parentDialog)->updateApplyButtonState();
}

void IpAccessControlDialog::onMoveDownButtonClick()
{
  int si = m_list.getSelectedIndex();
  // If nothing selected
  if ((si == -1) || (si == m_list.getCount() - 1)) {
    return ;
  }
  IpAccessRule *ip = (IpAccessRule *)m_list.getItemData(si);
  IpAccessRule *ipNext = (IpAccessRule *)m_list.getItemData(si + 1);

  setListViewItemText(si, ipNext);
  setListViewItemText(si + 1, ip);
  m_list.selectItem(si + 1);
  onIpCheckUpdate();
  ((ConfigDialog *)m_parentDialog)->updateApplyButtonState();
}

void IpAccessControlDialog::onListViewSelChange()
{
  int si = m_list.getSelectedIndex();
  updateButtonsState();
}

void IpAccessControlDialog::onAcceptRadioClick()
{
  if (!m_defaultActionAccept.isChecked()) {
    m_defaultActionAccept.check(true);
    m_defaultActionRefuse.check(false);
    ((ConfigDialog *)m_parentDialog)->updateApplyButtonState();
  }
}

void IpAccessControlDialog::onRefuseRadioClick()
{
  if (!m_defaultActionRefuse.isChecked()) {
    m_defaultActionRefuse.check(true);
    m_defaultActionAccept.check(false);
    ((ConfigDialog *)m_parentDialog)->updateApplyButtonState();
  }
}

void IpAccessControlDialog::onAllowLoopbackConnectionsClick()
{
  updateCheckBoxesState();
  updateButtonsState();
  ((ConfigDialog *)m_parentDialog)->updateApplyButtonState();
}

void IpAccessControlDialog::onAllowOnlyLoopbackConnectionsClick()
{
  updateCheckBoxesState();
  updateButtonsState();
  ((ConfigDialog *)m_parentDialog)->updateApplyButtonState();
}

void IpAccessControlDialog::onIpCheckUpdate()
{
  StringStorage ipStorage;
  m_ip.getText(&ipStorage);

   // Check if ip address is valid.

  if (!IpAccessRule::isIpAddressStringValid(ipStorage.getString())) {
    if (ipStorage.isEmpty()) {
      m_ipCheckResult.setText(StringTable::getString(IDS_ENTER_IP_HINT));
    } else {
      m_ipCheckResult.setText(StringTable::getString(IDS_BAD_IP_HINT));
    }
    return;
  }

  //
  // Convert ip to ansi string
  //

  AnsiStringStorage ansiIpStorage(&ipStorage);
  unsigned int addr = inet_addr(ansiIpStorage.getString());

  IpAccessRule::ActionType action = IpAccessRule::ACTION_TYPE_ALLOW;
  int rulesCount = 0;
  if (m_list.getCount() > 0) {
    rulesCount = m_list.getCount();
  }
  for (int i = 0; i < rulesCount; i++) {
    IpAccessRule *rule = (IpAccessRule *)m_list.getItemData(i);
    if (rule->isIncludingAddress(addr)) {
      action = rule->getAction();
      break;
    }
  }

  StringStorage actionDescription;
  actionDescription.setString(StringTable::getString(IDS_ACTION_UNDEF_HINT));
  switch (action) {
  case IpAccessRule::ACTION_TYPE_ALLOW:
    actionDescription.setString(StringTable::getString(IDS_ACTION_ACCEPT_HINT));
    break;
  case IpAccessRule::ACTION_TYPE_DENY:
    actionDescription.setString(StringTable::getString(IDS_ACTION_REJECT_HINT));
    break;
  case IpAccessRule::ACTION_TYPE_QUERY:
    actionDescription.setString(StringTable::getString(IDS_ACTION_QUERY_HINT));
    break;
  }

  m_ipCheckResult.setText(actionDescription.getString());
}

void IpAccessControlDialog::onQueryTimeoutUpdate()
{
  ((ConfigDialog *)m_parentDialog)->updateApplyButtonState();
}

void IpAccessControlDialog::updateButtonsState()
{
  /*if (m_onlyLoopbackConnections.isChecked()) {
    m_list.setEnabled(false);
    m_addButton.setEnabled(false);
    m_editButton.setEnabled(false);
    m_removeButton.setEnabled(false);
    m_moveDownButton.setEnabled(false);
    m_moveUpButton.setEnabled(false);
    return ;
  } else {
    m_list.setEnabled(true);
    m_addButton.setEnabled(true);
  }*/

  int si = m_list.getSelectedIndex();
  if (si == -1) {
    m_editButton.setEnabled(false);
    m_removeButton.setEnabled(false);
    m_moveUpButton.setEnabled(false);
    m_moveDownButton.setEnabled(false);
  } else {
    m_editButton.setEnabled(true);
    m_removeButton.setEnabled(true);
    if (si > 0) {
      m_moveUpButton.setEnabled(true);
    } else {
      m_moveUpButton.setEnabled(false);
    }
    if (si < m_list.getCount() - 1) {
      m_moveDownButton.setEnabled(true);
    } else {
      m_moveDownButton.setEnabled(false);
    }
  }
}

void IpAccessControlDialog::updateCheckBoxesState()
{
  if (m_allowLoopbackConnections.isChecked()) {
    m_onlyLoopbackConnections.setEnabled(true);
  } else {
    m_onlyLoopbackConnections.setEnabled(false);
    m_onlyLoopbackConnections.check(false);
  }
}

void IpAccessControlDialog::onListViewSelChangeDblClick()
{
  int si = m_list.getSelectedIndex();
  if (si == -1) {
    return ;
  } else {
    onEditButtonClick();
  }
}

void IpAccessControlDialog::setListViewItemText(int index, IpAccessRule *control)
{
  StringStorage firstIp;
  StringStorage lastIp;

  control->getFirstIp(&firstIp);
  control->getLastIp(&lastIp);

  m_list.setSubItemText(index, 0, firstIp.getString());
  m_list.setSubItemText(index, 1, lastIp.getString());
  switch (control->getAction()) {
  case IpAccessRule::ACTION_TYPE_ALLOW:
    m_list.setSubItemText(index, 2, StringTable::getString(IDS_ACTION_ACCEPT));
    break;
  case IpAccessRule::ACTION_TYPE_DENY:
    m_list.setSubItemText(index, 2, StringTable::getString(IDS_ACTION_DENY));
    break;
  case IpAccessRule::ACTION_TYPE_QUERY:
    m_list.setSubItemText(index, 2, StringTable::getString(IDS_ACTION_QUERY));
    break;
  }
  m_list.setItemData(index, (LPARAM)control);
}
