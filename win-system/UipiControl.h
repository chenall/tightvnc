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

#ifndef __UIPICONTROL_H__
#define __UIPICONTROL_H__

#include "util/CommonHeader.h"
#include "log-writer/LogWriter.h"

// This class allow to control the uipi for the current process.
class UipiControl
{
public:
  UipiControl(LogWriter *log);
  ~UipiControl();

  // This function allow to receive the message from a lower integrity
  // level sender. The function will take effect only for an application
  // running at Windows Vista and later.
  // @param hwnd - handle to a window that will be to receive the allowed
  // message (ignored at Windows Vista or older).
  // @throws Exception on a fail (Only for Vista and later).
  void allowMessage(UINT message, HWND hwnd);

private:
  LogWriter *m_log;
};

#endif // __UIPICONTROL_H__
