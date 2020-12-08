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

#include "QueryConnectionDialog.h"

#include "tvnserver/resource.h"

QueryConnectionDialog::QueryConnectionDialog(const TCHAR *peerAddress,
                                             bool acceptByDefault,
                                             DWORD timeOutInSec)
: m_acceptByDefault(acceptByDefault), m_timeout(timeOutInSec)
{
  m_peerAddress.setString(peerAddress);

  setResourceId(IDD_QUERY_RFB_CONNECTION);
}

QueryConnectionDialog::~QueryConnectionDialog()
{
}

BOOL QueryConnectionDialog::onInitDialog()
{
  initControls();

  m_peerAddressLabel.setText(m_peerAddress.getString());

  if (m_acceptByDefault) {
    m_acceptButton.setFocus();
  } else {
    m_rejectButton.setFocus();
  }

  setDefaultPushButton(m_acceptByDefault ? IDC_ACCEPT_BUTTON : IDC_REJECT_BUTTON);

  updateTimeoutLabel();

  SetTimer(m_ctrlThis.getWindow(), 0, 1000, 0);

  return FALSE;
}

BOOL QueryConnectionDialog::onNotify(UINT controlID, LPARAM data)
{
  return TRUE;
}

BOOL QueryConnectionDialog::onCommand(UINT controlID, UINT notificationID)
{
  switch (controlID) {
  case IDC_ACCEPT_BUTTON:
    onAccept();
    break;
  case IDC_REJECT_BUTTON:
    onReject();
    break;
  }
  return TRUE;
}

BOOL QueryConnectionDialog::onDestroy()
{
  return TRUE;
}

void QueryConnectionDialog::onMessageReceived(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  if (uMsg == WM_TIMER) {
    onTimer();
  }
}

void QueryConnectionDialog::initControls()
{
  HWND window = m_ctrlThis.getWindow();

  m_peerAddressLabel.setWindow(GetDlgItem(window, IDC_IP_EDIT));
  m_acceptButton.setWindow(GetDlgItem(window, IDC_ACCEPT_BUTTON));
  m_rejectButton.setWindow(GetDlgItem(window, IDC_REJECT_BUTTON));
  m_timeoutLabel.setWindow(GetDlgItem(window, IDC_TIMEOUT_LABEL));
}

void QueryConnectionDialog::onAccept()
{
  kill(ACCEPT_CHOISE);
}

void QueryConnectionDialog::onReject()
{
  kill(REJECT_CHOISE);
}

void QueryConnectionDialog::onTimer()
{
  if (m_timeout == 0) {
    KillTimer(m_ctrlThis.getWindow(), 0);

    if (m_acceptByDefault) {
      onAccept();
    } else {
      onReject();
    }
  } else {
    m_timeout--;

    updateTimeoutLabel();
  }
}

void QueryConnectionDialog::updateTimeoutLabel()
{
  StringStorage labelText;

  if (m_acceptByDefault) {
    labelText.format(StringTable::getString(IDS_AUTO_ACCEPT_CONNECTION_FORMAT), m_timeout);
  } else {
    labelText.format(StringTable::getString(IDS_AUTO_REJECT_CONNECTION_FORMAT), m_timeout);
  }

  m_timeoutLabel.setText(labelText.getString());
}
