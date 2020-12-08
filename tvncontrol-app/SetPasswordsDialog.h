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

#ifndef _SETPASSWORDSDIALOG_H_
#define _SETPASSWORDSDIALOG_H_

#include "util/winhdr.h"

#include "gui/BaseDialog.h"
#include "gui/CheckBox.h"
#include "gui/TextBox.h"
#include "gui/BalloonTip.h"

#include "win-system/RegistryKey.h"

#include "config-lib/ConnectionHistory.h"

// Set passwords dialog. Just allows user to set rfb and administrators
// passwords.
class SetPasswordsDialog : public BaseDialog
{
public:
  SetPasswordsDialog(bool initStateOfUseRfbAuth,
                     bool initStateOfUseAdminAuth);
  virtual ~SetPasswordsDialog();

  // Returns true if the checkbox has been checked
  bool getUseRfbPass();
  // Returns true if no rfb authentication is needed.
  bool getRfbPassForClear();
  void getRfbPass(StringStorage *pass);

  // Returns true if the checkbox has been checked
  bool getUseAdmPass();
  // Returns true if no admin authentication is needed.
  bool getAdmPassForClear();
  void getAdmPass(StringStorage *pass);

protected:
  void initControls();

  virtual BOOL onInitDialog();
  virtual BOOL onNotify(UINT controlID, LPARAM data);
  virtual BOOL onCommand(UINT controlID, UINT notificationID);
  virtual BOOL onDestroy();

  void onOkButtonClick();
  void onUseRfbAuthCheckBoxClick();
  void onProtectControlInterfaceCheckBoxClick();

protected:
  // UPdates internal state by the radio buttons.
  void readRadio();
  // Enables and disables the edit controls by the current state.
  void updateEditControls();

  CheckBox m_dontChangeRfbAuthSettingsRadio;
  CheckBox m_dontUseRfbAuthRadio;
  CheckBox m_useRfbAuthRadio;
  TextBox m_rfbPassEdit1;
  TextBox m_rfbPassEdit2;

  CheckBox m_dontChangeAdminAuthSettingsRadio;
  CheckBox m_dontUseAdminAuthRadio;
  CheckBox m_useAdminAuthRadio;
  TextBox m_admPassEdit1;
  TextBox m_admPassEdit2;

  // Tooltip
  BalloonTip m_passwordEmptyTooltip;
  BalloonTip m_passwordsNotMatchTooltip;

  bool m_dontChangeRfbAuth;
  bool m_useRfbAuth;
  StringStorage m_rfbPass;

  bool m_dontChangeAdmAuth;
  bool m_protectControlInterface;
  StringStorage m_admPass;
};

#endif // _SETPASSWORDSDIALOG_H_
