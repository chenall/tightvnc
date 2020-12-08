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

#ifndef _PASSWORD_CONTROL_H_
#define _PASSWORD_CONTROL_H_

#include "gui/Control.h"
#include <vector>

/**
Helper class that needed for managing one VNC-style password (set, change, unset password).
Common usage is to avoid code duplicate in control, primary, viewonly password management
from gui.
*/
class PasswordControl
{
public:
  /**
   * Creates new PasswordControl instance.
   * @param changeButton change password button.
   * @param unsetButton unset password button.
   */
  PasswordControl(Control *changeButton, Control *unsetButton);
  virtual ~PasswordControl();

  /**
  Enables or disables depending controls.
  @param enabled flag that to enable or disable controls.
  */
  void setEnabled(bool enabled);

  /**
   * Unsets password and updates dependent controls state.
   * @param promtUser determinates if need to promt user.
   * @param parentWindow hwnd of window to dialog message box (promt).
   */
  void unsetPassword(bool promtUser, HWND parentWindow);

  /**
  Updates password value inside this control and updates dependent controls state.
  @param plainText plain text password.
  */
  void setPassword(const TCHAR *plainText);

  /**
  Updates password value inside this control.
  @param cryptedPass crypted password.
  */
  void setCryptedPassword(const char *cryptedPass);

  /**
  Checks if password is set or unset.
  @return true if password is specified, false if password is unset.
  */
  bool hasPassword() const;

  /**
  Returns password, specified by user.
  @return crypted VNC-style password or 0 if no password specified.
  */
  const char *getCryptedPassword() const;

  /**
  Shows change password modal dialog and stores result to this password control object.
  @param parent control of parent dialog (optional, can be null).
  @return false if user cancels dialog, true otherwise.
  */
  bool showChangePasswordModalDialog(Control *parent);

private:
  void updateControlsState();
  void releaseCryptedPassword();

protected:
  Control *m_changeButton;
  Control *m_unsetButton;

  std::vector<char> m_cryptedPassword;

  bool m_enabled;
};

#endif
