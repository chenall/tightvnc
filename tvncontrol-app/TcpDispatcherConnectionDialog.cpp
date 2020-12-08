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
#include "win-system/Registry.h"
#include "TcpDispatcherConnectionDialog.h"
#include "util/StringParser.h"
#include "tvnserver-app/NamingDefs.h"
#include "tvnserver/resource.h"

TcpDispatcherConnectionDialog::TcpDispatcherConnectionDialog()
: BaseDialog(IDD_DISPATCHER_CONN),
  m_connHistoryKey(Registry::getCurrentUserKey(),
                   RegistryPaths::DISPATCHER_CONN_HISTORY_PATH,
                   true),
  m_connHistory(&m_connHistoryKey, 16),
  m_connectionId(0)
{
}

TcpDispatcherConnectionDialog::~TcpDispatcherConnectionDialog()
{
}

const TCHAR *TcpDispatcherConnectionDialog::getConnectString() const
{
  return m_connectString.getString();
}

const TCHAR *TcpDispatcherConnectionDialog::getDispatcherName() const
{
  return m_dispatcherName.getString();
}

const TCHAR *TcpDispatcherConnectionDialog::getKeyword() const
{
  return m_keyword.getString();
}

UINT32 TcpDispatcherConnectionDialog::getConnectionId() const
{
  return m_connectionId;
}

void TcpDispatcherConnectionDialog::initControls()
{
  HWND window = m_ctrlThis.getWindow();

  m_connectStringCB.setWindow(GetDlgItem(window, IDC_HOSTNAME_COMBO));
  m_dispatcherNameCB.setWindow(GetDlgItem(window, IDC_DISPATCH_NAME));
  m_keywordCB.setWindow(GetDlgItem(window, IDC_DISPATCH_KEYWORD));
  m_connectionIdCB.setWindow(GetDlgItem(window, IDC_DISPATCH_ID));
}

BOOL TcpDispatcherConnectionDialog::onInitDialog()
{
  initControls();

  // Load connection history.
  m_connHistory.load();

  for (size_t i = 0; i < m_connHistory.getHostCount(); i++) {
    m_connectStringCB.addItem(m_connHistory.getHost(i));
  }

  m_connectStringCB.setSelectedItem(0);
  m_connectStringCB.setFocus();

  return FALSE;
}

BOOL TcpDispatcherConnectionDialog::onNotify(UINT controlID, LPARAM data)
{
  return FALSE;
}

BOOL TcpDispatcherConnectionDialog::onCommand(UINT controlID, UINT notificationID)
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

BOOL TcpDispatcherConnectionDialog::onDestroy()
{
  return FALSE;
}

void TcpDispatcherConnectionDialog::onOkButtonClick()
{
  m_connectStringCB.getText(&m_connectString);
  m_dispatcherNameCB.getText(&m_dispatcherName);
  m_keywordCB.getText(&m_keyword);
  StringStorage connectionIdStr;
  m_connectionIdCB.getText(&connectionIdStr);

  if (!StringParser::parseUInt(connectionIdStr.getString(), &m_connectionId)) {
    MessageBox(0, _T("Invalid ID"), _T("Error"), MB_ICONERROR);
  } else {
    // Modify connection history.
    m_connHistory.addHost(m_connectString.getString());
    m_connHistory.save();
    m_connHistory.truncate();

    kill(IDOK);
  }

}

void TcpDispatcherConnectionDialog::onCancelButtonClick()
{
  kill(IDCANCEL);
}
