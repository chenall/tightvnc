// Copyright (C) 2011,2012 GlavSoft LLC.
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

#ifndef _LOGIN_DIALOG_H_
#define _LOGIN_DIALOG_H_

#include "client-config-lib/ViewerConfig.h"
#include "client-config-lib/ConnectionConfig.h"
#include "client-config-lib/ConnectionConfigSM.h"
#include "client-config-lib/ViewerSettingsManager.h"
#include "util/StringParser.h"
#include "gui/BaseDialog.h"
#include "gui/TextBox.h"
#include "gui/CheckBox.h"
#include "gui/ComboBox.h"
#include "gui/TrackBar.h"

#include "TvnViewer.h"
#include "resource.h"

class TvnViewer;

class LoginDialog : public BaseDialog
{
public:
  LoginDialog(TvnViewer *viewer);
  ~LoginDialog();

  // this function returns the host
  StringStorage getServerHost();
  void setConConf(ConnectionConfig *conConf);
  // set listening mode
  void setListening(bool isListening);

  static const int DEFAULT_PORT = 5900;

  // this constant will be returned if user have pressed 'Connect...' button
  static const int CANCEL_MODE = 0;
  // this constant will be returned if user have pressed 'Cancel' button
  static const int CONNECTION_MODE = 1;
  // this constant will be returned if user have pressed 'Listening' button
  static const int LISTENING_MODE = 2;

protected:
  BOOL onInitDialog();
  BOOL onCommand(UINT controlID, UINT notificationID);
  bool m_isListening;

  Control m_listening;
  Control m_ok;
  ConnectionConfig m_connectionConfig;
  ComboBox m_server;
  StringStorage m_serverHost;
  TvnViewer *m_viewer;

private:
  void enableConnect();
  void updateHistory();
  void onConnect();
  void onConfiguration();
  BOOL onOptions();
  void onOrder();
  void openUrl(const TCHAR *url);
  void onListening();
  void onAbout();
};

#endif
