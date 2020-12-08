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

#ifndef _NEW_FOLDER_DIALOG_H_
#define _NEW_FOLDER_DIALOG_H_

#include "gui/BaseDialog.h"
#include "gui/Control.h"
#include "gui/TextBox.h"

class NewFolderDialog : public BaseDialog
{
public:
  NewFolderDialog();
  NewFolderDialog(Control *parent);
  ~NewFolderDialog();

  void setFileName(const TCHAR *filename);
  void getFileName(StringStorage *storage);

protected:

  //
  // Inherited from BaseDialog
  //

  virtual BOOL onInitDialog();
  virtual BOOL onNotify(UINT controlID, LPARAM data);
  virtual BOOL onCommand(UINT controlID, UINT notificationID);
  virtual BOOL onDestroy();

  //
  // Button event handlers
  //

  void onOkButtonClick();
  void onCancelButtonClick();

private:

  void initControls();

protected:

  //
  // Controls
  //

  Control m_label;
  TextBox m_fileNameTextBox;

  StringStorage m_fileName;
};

#endif
