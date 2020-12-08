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

#include "AboutDialog.h"
#include "win-system/Shell.h"
#include "win-system/VersionInfo.h"
#include "win-system/Environment.h"
#include "gui/Control.h"
#include "BuildTime.h"
#include "resource.h"

AboutDialog::AboutDialog()
: BaseDialog(IDD_ABOUT_DIALOG)
{
}

AboutDialog::~AboutDialog()
{
}

void AboutDialog::onCloseButtonClick()
{
  kill(IDCANCEL);
}

void AboutDialog::onOrderSupportButtonClock()
{
  openUrl(StringTable::getString(IDS_URL_LICENSING_FVA));
}

void AboutDialog::onVisitSiteButtonClick()
{
  openUrl(StringTable::getString(IDS_URL_PRODUCT_FVA));
}

void AboutDialog::openUrl(const TCHAR *url)
{
  try {
    Shell::open(url, 0, 0);
  } catch (SystemException &sysEx) {
    StringStorage message;

    message.format(StringTable::getString(IDS_FAILED_TO_OPEN_URL_FORMAT), sysEx.getMessage());

    MessageBox(m_ctrlThis.getWindow(),
               message.getString(),
               StringTable::getString(IDS_MBC_TVNVIEWER),
               MB_OK | MB_ICONEXCLAMATION);
  }
}

BOOL AboutDialog::onInitDialog()
{
  // Update product version string.
  StringStorage versionString(_T("unknown"));
  try {
    StringStorage binaryPath;
    Environment::getCurrentModulePath(&binaryPath);
    VersionInfo productInfo(binaryPath.getString());
    versionString.setString(productInfo.getProductVersionString());
  } catch (SystemException &ex) {
    MessageBox(m_ctrlThis.getWindow(),
               ex.getMessage(),
               StringTable::getString(IDS_MBC_TVNVIEWER),
               MB_OK | MB_ICONEXCLAMATION);
  }

  // Format product version and build time for displaying on the dialog.
  StringStorage versionText;
  versionText.format(StringTable::getString(IDS_PRODUCT_VERSION_FORMAT),
                     versionString.getString(),
                     BuildTime::DATE);

  // Show version info on the dialog.
  Control versionLabel;
  versionLabel.setWindow(GetDlgItem(m_ctrlThis.getWindow(), IDC_STATIC_VERSION));
  versionLabel.setText(versionText.getString());

  // Show licensing info and/or special build info.
  Control licensingLabel;
  licensingLabel.setWindow(GetDlgItem(m_ctrlThis.getWindow(), IDC_STATIC_LICENSING));
  licensingLabel.setText(StringTable::getString(IDS_LICENSING_INFO));

  return FALSE;
}

BOOL AboutDialog::onNotify(UINT controlID, LPARAM data)
{
  return FALSE;
}

BOOL AboutDialog::onCommand(UINT controlID, UINT notificationID)
{
  switch (controlID) {
  case IDCANCEL:
    onCloseButtonClick();
    break;
  case IDC_ORDER_SUPPORT_BUTTON:
    onOrderSupportButtonClock();
    break;
  case IDC_VISIT_WEB_SITE_BUTTON:
    onVisitSiteButtonClick();
    break;
  }
  return FALSE;
}

BOOL AboutDialog::onDestroy()
{
  return FALSE;
}
