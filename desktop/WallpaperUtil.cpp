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

WallpaperUtil::WallpaperUtil(LogWriter *log)
: m_log(log)
{
  Configurator::getInstance()->addListener(this);
}

WallpaperUtil::~WallpaperUtil()
{
  Configurator::getInstance()->removeListener(this);

  try {
    Environment::restoreWallpaper(m_log);
    m_log->info(_T("Wallpaper was successfully restored"));
  } catch (Exception &e) {
    m_log->error(e.getMessage());
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
      Environment::disableWallpaper(m_log);
      m_log->info(_T("Wallpaper was successfully disabled"));
    } else {
      Environment::restoreWallpaper(m_log);
      m_log->info(_T("Wallpaper was successfully restored"));
    }
  } catch (Exception &e) {
    m_log->error(e.getMessage());
  }
}
