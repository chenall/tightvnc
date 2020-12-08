// Copyright (C) 2012 GlavSoft LLC.
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

#include "FsWarningDialog.h"
#include "client-config-lib/ViewerConfig.h"
#include "client-config-lib/ViewerSettingsManager.h"

FsWarningDialog::FsWarningDialog()
: BaseDialog(IDD_FS_WARNING)
{
}

BOOL FsWarningDialog::onInitDialog()
{
  setControlById(m_fsWarning, IDC_CWARN_FS);

  m_fsWarning.check(false);
  return FALSE;
}

BOOL FsWarningDialog::onCommand(UINT controlID, UINT notificationID)
{
  if (controlID == IDOK) {
    ViewerConfig *config = ViewerConfig::getInstance();
    bool promt = !m_fsWarning.isChecked();
    config->promptOnFullscreen(promt);
    config->saveToStorage(ViewerSettingsManager::getInstance());
    kill(1);
    return TRUE;
  }
  if (controlID == IDCANCEL) {
    kill(0);
    return TRUE;
  }
  return FALSE;
}
