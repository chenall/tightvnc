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

#include "NewFolderDialog.h"
#include "resource.h"

NewFolderDialog::NewFolderDialog()
{
  setResourceId(ftclient_createFolderDialog);
  m_fileName.setString(_T(""));
}

NewFolderDialog::NewFolderDialog(Control *parent)
{
  setResourceId(ftclient_createFolderDialog);
  setParent(parent);
  m_fileName.setString(_T(""));
}

NewFolderDialog::~NewFolderDialog()
{
}

void NewFolderDialog::setFileName(const TCHAR *filename)
{
  m_fileName.setString(filename);
}

void NewFolderDialog::getFileName(StringStorage *storage)
{
  *storage = m_fileName;
}

BOOL NewFolderDialog::onInitDialog()
{
  initControls();
  m_fileNameTextBox.setText(m_fileName.getString());
  return TRUE;
}

BOOL NewFolderDialog::onNotify(UINT controlID, LPARAM data)
{
  return TRUE;
}

BOOL NewFolderDialog::onCommand(UINT controlID, UINT notificationID)
{
  switch (controlID) {
  case IDOK:
    onOkButtonClick();
    break;
  case IDCANCEL:
    onCancelButtonClick();
    break;
  }
  return TRUE;
}

BOOL NewFolderDialog::onDestroy()
{
  return TRUE;
}

void NewFolderDialog::onOkButtonClick()
{
  StringStorage fileName;

  m_fileNameTextBox.getText(&fileName);

  if (fileName.isEmpty() || (fileName.findOneOf(_T("\\/")) != -1)) {
    MessageBox(m_ctrlThis.getWindow(),
               _T("File name cannot be empty and cannot contain '/' or '\\' characters."),
               _T("Incorrect File Name"),
               MB_OK | MB_ICONWARNING);
    m_fileNameTextBox.setFocus();
    return ;
  }

  m_fileName = fileName;

  kill(IDOK);
}

void NewFolderDialog::onCancelButtonClick()
{
  kill(IDCANCEL);
}

void NewFolderDialog::initControls()
{
  HWND hwnd = m_ctrlThis.getWindow();

  m_label.setWindow(GetDlgItem(hwnd, IDC_LABEL));
  m_fileNameTextBox.setWindow(GetDlgItem(hwnd, IDC_FILENAME_EDIT));
}
