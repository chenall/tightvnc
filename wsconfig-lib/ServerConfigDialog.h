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

#ifndef _SERVER_CONFIG_DIALOG_H_
#define _SERVER_CONFIG_DIALOG_H_

#include "gui/BaseDialog.h"
#include "gui/TextBox.h"
#include "gui/CheckBox.h"
#include "gui/SpinControl.h"
#include "gui/BalloonTip.h"

#include "server-config-lib/ServerConfig.h"
#include "PasswordControl.h"

class ServerConfigDialog : public BaseDialog
{
public:
  ServerConfigDialog();
  virtual ~ServerConfigDialog();

  void setParentDialog(BaseDialog *dialog);

public:

  //
  // BaseDialog overrided methods
  //

  virtual BOOL onInitDialog();
  virtual BOOL onNotify(UINT controlID, LPARAM data);
  virtual BOOL onCommand(UINT controlID, UINT notificationID);
  virtual BOOL onDestroy() { return TRUE; }

  //
  // Helper methods
  //

  bool validateInput();
  void updateUI();
  void apply();

private:
  void initControls();
  void updateControlDependencies();

  //
  // Contol event handlers
  //

  void onAcceptRfbConnectionsClick();
  void onAcceptHttpConnectionsClick();
  void onAuthenticationClick();
  void onPrimaryPasswordChange();
  void onReadOnlyPasswordChange();
  void onUnsetPrimaryPasswordClick();
  void onUnsetReadOnlyPasswordClick();
  void onShowTrayIconCheckBoxClick();
  void onPollingIntervalSpinChangePos(LPNMUPDOWN message);
  void onRfbPortUpdate();
  void onHttpPortUpdate();
  void onUrlParamsClick();
  void onPollingIntervalUpdate();
  void onFileTransferCheckBoxClick();
  void onRemoveWallpaperCheckBoxClick();
  void onGrabTransparentWindowsChanged();
  void onUseD3DChanged();

  //
  // Input handling
  //

  void onBlockLocalInputChanged();
  void onBlockRemoteInputChanged();
  void onLocalInputPriorityChanged();
  void onInactivityTimeoutUpdate();
  void updateCheckboxesState();

protected:
  // Configuration
  ServerConfig *m_config;
  // Controls
  TextBox m_rfbPort;
  TextBox m_httpPort;
  TextBox m_pollingInterval;
  CheckBox m_useD3D;
  CheckBox m_useMirrorDriver;
  CheckBox m_enableFileTransfers;
  CheckBox m_removeWallpaper;
  CheckBox m_acceptRfbConnections;
  CheckBox m_acceptHttpConnections;
  CheckBox m_showTrayIcon;
  Control m_primaryPassword;
  Control m_readOnlyPassword;
  Control m_unsetPrimaryPassword;
  Control m_unsetReadOnlyPassword;
  CheckBox m_useAuthentication;
  SpinControl m_rfbPortSpin;
  SpinControl m_httpPortSpin;
  SpinControl m_pollingIntervalSpin;

  //
  // Begin of input handling members
  //

  CheckBox m_blockRemoteInput;
  CheckBox m_blockLocalInput;
  CheckBox m_localInputPriority;
  TextBox m_localInputPriorityTimeout;
  SpinControl m_inactivityTimeoutSpin;

  //
  // End of input handling members
  //

  // Error notifications
  BaseDialog *m_parentDialog;

  // Primary password control.
  PasswordControl *m_ppControl;
  PasswordControl *m_vpControl;
};

#endif
