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

#include "WallpaperUtil.h"
#include "win-system/Environment.h"
#include "server-config-lib/Configurator.h"
#include "win-system/AutoImpersonator.h"

WallpaperUtil::WallpaperUtil(LogWriter *log)
: m_wasDisabled(false), 
  m_log(log)
{
  Configurator::getInstance()->addListener(this);
}

WallpaperUtil::~WallpaperUtil()
{
  Configurator::getInstance()->removeListener(this);
  if (m_wasDisabled) {
    try {
      restoreWallpaper();
      m_log->info(_T("Wallpaper was successfully restored"));
    }
    catch (Exception &e) {
      m_log->error(e.getMessage());
    }
  }
}

void WallpaperUtil::onConfigReload(ServerConfig *serverConfig)
{
  updateWallpaper();
}

void WallpaperUtil::updateWallpaper()
{
  try {
    ServerConfig *srvConf = Configurator::getInstance()->getServerConfig();
    if (srvConf->isRemovingDesktopWallpaperEnabled()) {
      disableWallpaper();
      m_wasDisabled = true;
      m_log->info(_T("Wallpaper was successfully disabled"));
    } else {
      if (m_wasDisabled) {
        restoreWallpaper();
        m_log->info(_T("Wallpaper was successfully restored"));
        m_wasDisabled = false;
      }
    }
  } catch (Exception &e) {
    m_log->error(e.getMessage());
  }
}

void WallpaperUtil::restoreWallpaper()
{
  // FIXME: Remove log from here. Log only from caller.
  m_log->info(_T("Try to restore wallpaper"));
  Impersonator imp(m_log);
  AutoImpersonator ai(&imp, m_log);
  int result;

  if (m_wallparerPath.getLength() == 0) {
    result = SystemParametersInfo(SPI_SETDESKWALLPAPER, 0, 0, 0);
  }
  else {
    result = SystemParametersInfo(SPI_SETDESKWALLPAPER, m_wallparerPath.getSize(), (void *)m_wallparerPath.getString(), 0);
  }

  if (result == 0) {
    throw SystemException(_T("Cannot restore desktop wallpaper"));
  }
}

void WallpaperUtil::disableWallpaper()
{
  m_log->info(_T("Try to disable wallpaper"));
  Impersonator imp(m_log);
  AutoImpersonator ai(&imp, m_log);
  TCHAR path[MAX_PATH] = _T("");

  if (SystemParametersInfo(SPI_GETDESKWALLPAPER, MAX_PATH, path, 0) == 0) {
    path[0] = '\0';
  }

  if (SystemParametersInfo(SPI_SETDESKWALLPAPER, 0, _T(""), 0) == 0) {
    throw SystemException(_T("Cannot disable desktop wallpaper"));
  }
  m_wallparerPath = StringStorage(path);
}
