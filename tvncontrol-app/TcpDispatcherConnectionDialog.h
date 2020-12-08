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

#ifndef __TCPDISPATCHERCONNECTIONDIALOG_H__
#define __TCPDISPATCHERCONNECTIONDIALOG_H__

#include "util/winhdr.h"

#include "gui/BaseDialog.h"
#include "gui/ComboBox.h"

#include "win-system/RegistryKey.h"

#include "config-lib/ConnectionHistory.h"

 // Tcp dispatcher connection dialog. Just allows user to put connection string
class TcpDispatcherConnectionDialog : public BaseDialog
{
public:
  TcpDispatcherConnectionDialog();
  virtual ~TcpDispatcherConnectionDialog();

   // Gets connection string, specified by user.
   // @return connection string.
  const TCHAR *getConnectString() const;
  const TCHAR *getDispatcherName() const;
  const TCHAR *getKeyword() const;
  UINT32 getConnectionId() const;

protected:
  void initControls();

  virtual BOOL onInitDialog();
  virtual BOOL onNotify(UINT controlID, LPARAM data);
  virtual BOOL onCommand(UINT controlID, UINT notificationID);
  virtual BOOL onDestroy();

  void onOkButtonClick();
  void onCancelButtonClick();

protected:
   // Member to hold strings after dialog is destroyed.
  StringStorage m_connectString;
  StringStorage m_dispatcherName;
  StringStorage m_keyword;
  UINT32 m_connectionId;

  /**
   * Connection history registry key.
   */
  RegistryKey m_connHistoryKey;
  /**
   * Connection history container.
   */
  ConnectionHistory m_connHistory;

  /**
   * Combo box with connection history.
   */
  ComboBox m_connectStringCB;
  ComboBox m_dispatcherNameCB;
  ComboBox m_keywordCB;
  ComboBox m_connectionIdCB;
};

#endif // __TCPDISPATCHERCONNECTIONDIALOG_H__
