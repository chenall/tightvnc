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

#ifndef _PORT_MAPPING_DIALOG_H_
#define _PORT_MAPPING_DIALOG_H_

#include "gui/BaseDialog.h"
#include "gui/Control.h"
#include "gui/ListBox.h"

#include "server-config-lib/Configurator.h"

class PortMappingDialog : public BaseDialog
{
public:
  PortMappingDialog();
  virtual ~PortMappingDialog();

  void setParentDialog(BaseDialog *dialog);

protected:

  //
  // Inherited from BaseDialog.
  //

  virtual BOOL onInitDialog();
  virtual BOOL onCommand(UINT controlID, UINT notificationID);
  virtual BOOL onNotify(UINT controlID, LPARAM data) { return TRUE; }
  virtual BOOL onDestroy() { return TRUE; }

  //
  // Controls event handlers.
  //

  void onAddButtonClick();
  void onEditButtonClick();
  void onRemoveButtonClick();
  void onExPortsListBoxSelChange();
  void onExPortsListBoxDoubleClick();

private:
  void initControls();

protected:
  ListBox m_exPortsListBox;
  Control m_editButton;
  Control m_removeButton;

  PortMappingContainer *m_extraPorts;

  BaseDialog *m_parent;
};

#endif
