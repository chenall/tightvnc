// Copyright (C) 2008,2009,2010,2011,2012 GlavSoft LLC.
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

#ifndef __DESKTOPWINIMPL_H__
#define __DESKTOPWINIMPL_H__

#include "util/CommonHeader.h"
#include "DesktopConfigLocal.h"
#include "desktop/WallpaperUtil.h"
#include "thread/GuiThread.h"
#include "DesktopBaseImpl.h"
#include "Win32ScreenDriverFactory.h"
#include "log-writer/LogWriter.h"

class DesktopWinImpl : public GuiThread, public DesktopBaseImpl
{
public:
  DesktopWinImpl(ClipboardListener *extClipListener,
             UpdateSendingListener *extUpdSendingListener,
             AbnormDeskTermListener *extDeskTermListener,
             LogWriter *log);
  virtual ~DesktopWinImpl();

protected:
  virtual void execute();
  virtual void onTerminate();

private:
  void freeResource();

  // Writes some desktop info to log.
  void logDesktopInfo();

  virtual bool isRemoteInputTempBlocked();
  virtual void applyNewConfiguration();

  Win32ScreenDriverFactory m_scrDriverFactory;

  WallpaperUtil *m_wallPaper;

  DesktopConfigLocal *m_deskConf;

  LogWriter *m_log;
};

#endif // __DESKTOPWINIMPL_H__
