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

#include "util/winhdr.h"
#include "tvnserver-app/NamingDefs.h"

#include "win-system/Registry.h"

#include "OutgoingConnectionDialog.h"

#include "tvnserver/resource.h"

OutgoingConnectionDialog::OutgoingConnectionDialog()
: BaseDialog(IDD_OUTGOING_CONN),
  m_connHistoryKey(Registry::getCurrentUserKey(),
                   RegistryPaths::SERVER_REVERSE_CONN_HISTORY_PATH,
                   true),
  m_connHistory(&m_connHistoryKey, 16)
{
}

OutgoingConnectionDialog::~OutgoingConnectionDialog()
{
}

const TCHAR *OutgoingConnectionDialog::getConnectString() const
{
  return m_connectString.getString();
}

bool OutgoingConnectionDialog::isViewOnly() const
{
  return m_isViewOnly;
}

void OutgoingConnectionDialog::initControls()
{
  HWND window = m_ctrlThis.getWindow();

  m_connectStringCB.setWindow(GetDlgItem(window, IDC_HOSTNAME_COMBO));
  m_viewOnlyCB.setWindow(GetDlgItem(window, IDC_VIEW_ONLY_CHECKBOX));
}

BOOL OutgoingConnectionDialog::onInitDialog()
{
  initControls();

  m_viewOnlyCB.check(false);

  // Load connection history.

  m_connHistory.load();

  for (size_t i = 0; i < m_connHistory.getHostCount(); i++) {
    m_connectStringCB.addItem(m_connHistory.getHost(i));
  }

  m_connectStringCB.setSelectedItem(0);
  m_connectStringCB.setFocus();

  return FALSE;
}

BOOL OutgoingConnectionDialog::onNotify(UINT controlID, LPARAM data)
{
  return FALSE;
}

BOOL OutgoingConnectionDialog::onCommand(UINT controlID, UINT notificationID)
{
  switch (controlID) {
  case IDOK:
    onOkButtonClick();
    break;
  case IDCANCEL:
    onCancelButtonClick();
    break;
  }
  return FALSE;
}

BOOL OutgoingConnectionDialog::onDestroy()
{
  return FALSE;
}

void OutgoingConnectionDialog::onOkButtonClick()
{
  m_connectStringCB.getText(&m_connectString);

  m_isViewOnly = m_viewOnlyCB.isChecked();

  // Modify connection history.

  m_connHistory.addHost(m_connectString.getString());
  m_connHistory.save();
  m_connHistory.truncate();

  kill(IDOK);
}

void OutgoingConnectionDialog::onCancelButtonClick()
{
  kill(IDCANCEL);
}
