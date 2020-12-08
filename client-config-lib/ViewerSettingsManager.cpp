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

#include "ViewerSettingsManager.h"

#include "util/Exception.h"
#include "win-system/Registry.h"

SettingsManager *ViewerSettingsManager::s_instance = 0;

ViewerSettingsManager::ViewerSettingsManager(const TCHAR registryName[])
: RegistrySettingsManager()
{
  StringStorage keyName;
  keyName.format(_T("%s\\Settings\\"), registryName);
  setRegistryKey(Registry::getCurrentUserKey(), keyName.getString());
}

ViewerSettingsManager::~ViewerSettingsManager()
{
}

void ViewerSettingsManager::initInstance(const TCHAR registryName[])
{
  if (s_instance != 0)
    delete s_instance;
  s_instance = new ViewerSettingsManager(registryName);
}

SettingsManager *ViewerSettingsManager::getInstance()
{
  if (s_instance == 0) {
    throw Exception(_T("Instance of viewer settings manager is 0"));
  }
  return s_instance;
}
