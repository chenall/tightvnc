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

#ifndef _EDIT_IP_ACCESS_RULE_DIALOG_H_
#define _EDIT_IP_ACCESS_RULE_DIALOG_H_

#include "gui/BaseDialog.h"
#include "gui/TextBox.h"
#include "gui/CheckBox.h"
#include "gui/BalloonTip.h"

#include "server-config-lib/IpAccessRule.h"

#include "util/StringStorage.h"

class EditIpAccessRuleDialog : public BaseDialog
{
public:
  EditIpAccessRuleDialog();
  virtual ~EditIpAccessRuleDialog();

  void setIpAccessControl(IpAccessRule *iac) { m_data = iac; }
  IpAccessRule *getIpAccessControl() { return m_data; }

  void setEditFlag(bool flagEnabled);

protected:

  //
  // BaseDialog overrided methods
  //

  virtual BOOL onInitDialog();
  virtual BOOL onCommand(UINT cID, UINT nID);
  virtual BOOL onNotify(UINT controlID, LPARAM data) { return TRUE; }
  virtual BOOL onDestroy() { return TRUE; }

  //
  // Controls event handlers
  //

  void onOkButtonClick();
  void onCancelButtonClick();
  void onAccessTypeRadioClick(int num);
private:
  void initControls();
  bool validateInput();

protected:
  // Controls
  TextBox m_firstIp;
  TextBox m_lastIp;
  CheckBox m_access[3];
  // Data
  IpAccessRule *m_data;
  bool m_isOpenedForEdit;

  //
  // Balloon tips with warning when user writes incorrect value
  // to textbox
  //

  BalloonTip m_warningBalloonTip;
  BalloonTip m_lastIpLessThanFirstBT;
};

#endif
