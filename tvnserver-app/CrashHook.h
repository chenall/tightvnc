// Copyright (C) 2010,2011,2012 GlavSoft LLC.
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

#ifndef __CRASHHOOK_H__
#define __CRASHHOOK_H__

#include "util/CommonHeader.h"
#include "util/Singleton.h"
#include "thread/LocalMutex.h"
#include "win-system/ApplicationCrashEvents.h"

class CrashHook : private Singleton<CrashHook>
{
public:
  CrashHook(ApplicationCrashEvents *notifier);
  virtual ~CrashHook();

  // Call this function to enables GUI on application crash.
  void setGuiEnabled();

  // Call this function to set the HKLM registry root to write an auxiliary
  // information. Default value is HKEY_CURRENT_USER.
  void setHklmRoot();

private:
  static LONG WINAPI topLevelExceptionFilter(_EXCEPTION_POINTERS *pExceptionInfo);

  static bool m_guiEnabled;
  static HKEY m_rootHkey;
  static LocalMutex m_guiEnabledMutex;

  static ApplicationCrashEvents *m_notifier;
};

#endif // __CrashHook_H__
