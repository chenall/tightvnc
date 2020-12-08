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

#ifndef _EDIT_PORT_MAPPING_DIALOG_H_
#define _EDIT_PORT_MAPPING_DIALOG_H_

#include "gui/BaseDialog.h"
#include "gui/TextBox.h"

#include "server-config-lib/PortMapping.h"

class EditPortMappingDialog : public BaseDialog
{
public:
  typedef enum {
    Add   = 0x0,
    Edit  = 0x1
 } DialogType;

public:
  EditPortMappingDialog(DialogType dlgType);
  virtual ~EditPortMappingDialog();

public:
  void setMapping(PortMapping *mapping);
protected:
  void initControls();
  bool isUserDataValid();

  //
  // Inherited from BaseDialog
  //

  virtual BOOL onInitDialog();
  virtual BOOL onCommand(UINT cID, UINT nID);
  virtual BOOL onNotify(UINT controlID, LPARAM data) { return TRUE; }
  virtual BOOL onDestroy() { return TRUE; }

  void onOkButtonClick();
  void onCancelButtonClick();
protected:
  TextBox m_geometryTextBox;
  TextBox m_portTextBox;
  DialogType m_dialogType;
  PortMapping *m_mapping;
};

#endif
