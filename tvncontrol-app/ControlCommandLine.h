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

#ifndef _CONTROL_COMMAND_LINE_H_
#define _CONTROL_COMMAND_LINE_H_

#include "util/CommandLine.h"
#include "util/CommandLineFormatException.h"

#include "region/Rect.h"

class ControlCommandLine : private CommandLine
{
public:
  static const TCHAR CONFIG_APPLICATION[];
  static const TCHAR CONFIG_SERVICE[];

  static const TCHAR CONTROL_SERVICE[];
  static const TCHAR CONTROL_APPLICATION[];
  static const TCHAR PASSWORD_FILE[];
  static const TCHAR CONFIG_RELOAD[];
  static const TCHAR DISCONNECT_ALL[];
  static const TCHAR CONNECT[];
  static const TCHAR DISPATCH[];
  static const TCHAR SHUTDOWN[];
  static const TCHAR SHARE_PRIMARY[];
  static const TCHAR SHARE_RECT[];
  static const TCHAR SHARE_DISPLAY[];
  static const TCHAR SHARE_WINDOW[];
  static const TCHAR SHARE_FULL[];
  static const TCHAR SHARE_APP[];

  static const TCHAR SET_CONTROL_PASSWORD[];
  static const TCHAR SET_PRIMARY_VNC_PASSWORD[];
  static const TCHAR CHECK_SERVICE_PASSWORDS[];

  static const TCHAR SLAVE_MODE[];
  static const TCHAR DONT_ELEVATE[];

public:
  ControlCommandLine();
  virtual ~ControlCommandLine();

  void parse(const CommandLineArgs *cmdArgs) throw(CommandLineFormatException);

  void getPasswordFile(StringStorage *passwordFile) const;
  bool hasPasswordFile();
  bool hasReloadFlag();
  bool hasKillAllFlag();
  bool hasConnectFlag();
  bool hasDispatchFlag();
  void getConnectHostName(StringStorage *hostName) const;
  void getDispatcherSpec(StringStorage *dispatcherSpec) const;
  bool hasShutdownFlag();
  bool hasSetVncPasswordFlag();
  bool hasSetControlPasswordFlag();
  bool hasCheckServicePasswords();
  bool hasConfigAppFlag();
  bool hasConfigServiceFlag();
  bool hasControlServiceFlag();
  bool hasControlAppFlag();
  bool hasDontElevateFlag();
  bool isSlave();

  bool hasSharePrimaryFlag();
  bool hasShareRect();
  bool hasShareDisplay();
  bool hasShareWindow();
  bool hasShareFull();
  bool hasShareApp();
  unsigned char getShareDisplayNumber();
  void getShareWindowName(StringStorage *out);
  Rect getShareRect();
  unsigned int getSharedAppProcessId();

  const TCHAR *getPrimaryVncPassword() const;
  const TCHAR *getControlPassword() const;

  bool isCommandSpecified();

private:
  void parseRectCoordinates(const StringStorage *strCoord);
  void parseDisplayNumber(const StringStorage *strDispNumber);
  void parseProcessId(const StringStorage *str);

  StringStorage m_vncPassword;
  StringStorage m_controlPassword;

  StringStorage m_connectHostName;
  StringStorage m_dispatcherSpec;
  StringStorage m_passwordFile;

  Rect m_shareRect;
  unsigned char m_displayNumber;
  StringStorage m_windowHeaderName;
  unsigned int m_sharedAppProcessId;
};

#endif
