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

#include "FileExistDialog.h"
#include "util/DateTime.h"
#include "resource.h"
#include <crtdbg.h>
#include <stdio.h>

FileExistDialog::FileExistDialog()
: m_controlsInitialized(false), 
  m_newFileInfo(NULL),
  m_existingFileInfo(NULL), 
  m_canAppend(true)
{
  setResourceId(ftclient_fileExistDialog);
  resetDialogResultValue();
}

FileExistDialog::~FileExistDialog()
{
}

int FileExistDialog::showModal()
{
  if (m_skipAll) {
    return SKIP_RESULT;
  } else if (m_overwriteAll) {
    return OVERWRITE_RESULT;
  }
  return BaseDialog::showModal();
}

void FileExistDialog::setFilesInfo(FileInfo *existingFileInfo, FileInfo *newFileInfo,
                                   const TCHAR *pathToFileCaption)
{
  m_newFileInfo = newFileInfo;
  m_existingFileInfo = existingFileInfo;

  m_pathToFileCaption.setString(pathToFileCaption);

  _ASSERT(m_newFileInfo != NULL);
  _ASSERT(m_existingFileInfo != NULL);

  m_canAppend = (m_existingFileInfo->getSize() < m_newFileInfo->getSize());

  if (m_controlsInitialized) {
    updateGui(newFileInfo, &m_newSizeLabel, &m_newModTimeLabel);
    m_fileNameLabel.setText(pathToFileCaption);
    m_appendButton.setEnabled(m_canAppend);
  }
}

void FileExistDialog::resetDialogResultValue()
{
  m_overwriteAll = false;
  m_skipAll = false;
}

BOOL FileExistDialog::onInitDialog()
{
  initControls();

  _ASSERT(m_newFileInfo != NULL);
  _ASSERT(m_existingFileInfo != NULL);

  updateGui(m_newFileInfo, &m_newSizeLabel, &m_newModTimeLabel);
  updateGui(m_existingFileInfo, &m_existingSizeLabel, &m_existingModTimeLabel);

  m_appendButton.setEnabled(m_canAppend);

  m_fileNameLabel.setText(m_pathToFileCaption.getString());

  return TRUE;
}

BOOL FileExistDialog::onNotify(UINT controlID, LPARAM data)
{
  return TRUE;
}

BOOL FileExistDialog::onCommand(UINT controlID, UINT notificationID)
{
  switch (controlID) {
  case IDC_OVERWRITE_BUTTON:
    onOverwriteButtonClick();
    break;
  case IDC_OVERWRITE_ALL_BUTTON:
    onOverwriteAllButtonClick();
    break;
  case IDC_SKIP_BUTTON:
    onSkipButtonClick();
    break;
  case IDC_SKIP_ALL_BUTTON:
    onSkipAllButtonClick();
    break;
  case IDC_APPEND_BUTTON:
    onAppendButtonClick();
    break;
  case IDC_CANCEL_BUTTON:
    onCancelButtonClick();
    break;
  }
  return TRUE;
}

BOOL FileExistDialog::onDestroy()
{
  return TRUE;
}

void FileExistDialog::onOverwriteButtonClick()
{
  kill(OVERWRITE_RESULT);
}

void FileExistDialog::onOverwriteAllButtonClick()
{
  kill(OVERWRITE_RESULT);
  m_overwriteAll = true;
}

void FileExistDialog::onSkipButtonClick()
{
  kill(SKIP_RESULT);
}

void FileExistDialog::onSkipAllButtonClick()
{
  kill(SKIP_RESULT);
  m_skipAll = true;
}

void FileExistDialog::onAppendButtonClick()
{
  kill(APPEND_RESULT);
}

//
// FIXME: Really it cannot break operation execution for now
//

void FileExistDialog::onCancelButtonClick()
{
  kill(CANCEL_RESULT);
}

void FileExistDialog::updateGui(FileInfo *fileInfo, Control *sizeLabel, Control *modTimeLabel)
{
  TCHAR buffer[255];
  _stprintf_s(&buffer[0], 255, _T("%ld bytes"), fileInfo->getSize());
  sizeLabel->setText(&buffer[0]);

  DateTime dateTime(fileInfo->lastModified());

  StringStorage formatTimeString;
  dateTime.toString(&formatTimeString);

  modTimeLabel->setText(formatTimeString.getString());
}

void FileExistDialog::initControls()
{
  HWND hwnd = m_ctrlThis.getWindow();

  m_fileNameLabel.setWindow(GetDlgItem(hwnd, IDC_FILENAME_LABEL));
  m_newSizeLabel.setWindow(GetDlgItem(hwnd, IDC_SIZE1_LABEL));
  m_newModTimeLabel.setWindow(GetDlgItem(hwnd, IDC_DATE1_LABEL));
  m_existingSizeLabel.setWindow(GetDlgItem(hwnd, IDC_SIZE2_LABEL));
  m_existingModTimeLabel.setWindow(GetDlgItem(hwnd, IDC_DATE2_LABEL));

  m_appendButton.setWindow(GetDlgItem(hwnd, IDC_APPEND_BUTTON));

  m_controlsInitialized = true;
}
