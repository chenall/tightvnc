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

#include "tvnserver/resource.h"
#include "EditPortMappingDialog.h"

#include "util/StringParser.h"

#include "server-config-lib/Configurator.h"
#include "server-config-lib/PortMappingContainer.h"

EditPortMappingDialog::EditPortMappingDialog(DialogType dlgType)
: BaseDialog(IDD_EDIT_PORT_MAPPING), m_dialogType(dlgType)
{
}

EditPortMappingDialog::~EditPortMappingDialog()
{
}

void EditPortMappingDialog::setMapping(PortMapping *mapping)
{
  m_mapping = mapping;
}

void EditPortMappingDialog::onCancelButtonClick()
{
  kill(IDCANCEL);
}

void EditPortMappingDialog::onOkButtonClick()
{
  if (!isUserDataValid())
    return ;

  //
  // Temporary variables
  //

  PortMappingRect rect;
  int port;

  StringStorage portStringStorage;
  StringStorage rectStringStorage;

  m_geometryTextBox.getText(&rectStringStorage);
  m_portTextBox.getText(&portStringStorage);

  PortMappingRect::parse(rectStringStorage.getString(), &rect);
  StringParser::parseInt(portStringStorage.getString(), &port);

  m_mapping->setPort(port);
  m_mapping->setRect(rect);

  kill(IDOK);
}

void EditPortMappingDialog::initControls()
{
  HWND dialogHwnd = m_ctrlThis.getWindow();
  m_geometryTextBox.setWindow(GetDlgItem(dialogHwnd, IDC_GEOMETRY_EDIT));
  m_portTextBox.setWindow(GetDlgItem(dialogHwnd, IDC_PORT_EDIT));
}

bool EditPortMappingDialog::isUserDataValid()
{
  StringStorage rectStringStorage;
  StringStorage portStringStorage;

  m_geometryTextBox.getText(&rectStringStorage);
  m_portTextBox.getText(&portStringStorage);

  if (!PortMappingRect::tryParse(rectStringStorage.getString())) {
    MessageBox(m_ctrlThis.getWindow(),
               StringTable::getString(IDS_INVALID_PORT_MAPPING_STRING),
               StringTable::getString(IDS_CAPTION_BAD_INPUT),
               MB_OK | MB_ICONWARNING);
    m_geometryTextBox.setFocus();
    return false;
  }

  int port;

  StringParser::parseInt(portStringStorage.getString(), &port);

  if ((port < 1) || (port > 65535)) {
    MessageBox(m_ctrlThis.getWindow(),
               StringTable::getString(IDS_PORT_RANGE_ERROR),
               StringTable::getString(IDS_CAPTION_BAD_INPUT),
               MB_OK | MB_ICONWARNING);
    m_portTextBox.setFocus();
    return false;
  }

  PortMappingContainer *extraPorts = Configurator::getInstance()->getServerConfig()->getPortMappingContainer();

  size_t index = extraPorts->findByPort(port);

  if ((index != (size_t)-1) && (extraPorts->at(index) != m_mapping)) {
    MessageBox(m_ctrlThis.getWindow(),
               StringTable::getString(IDS_PORT_ALREADY_IN_USE),
               StringTable::getString(IDS_CAPTION_BAD_INPUT),
               MB_OK | MB_ICONWARNING);
    m_portTextBox.setFocus();
    return false;
  }

  return true;
}

BOOL EditPortMappingDialog::onInitDialog()
{
  initControls();

  if (m_dialogType == Add) {
    m_portTextBox.setText(_T("5901"));
    m_geometryTextBox.setText(_T("640x480+0+0"));
  } else if (m_dialogType == Edit) {
    StringStorage portString;
    StringStorage rectString;

    portString.format(_T("%d"), m_mapping->getPort());
    m_mapping->getRect().toString(&rectString);

    m_portTextBox.setText(portString.getString());
    m_geometryTextBox.setText(rectString.getString());
  }

  return TRUE;
}

BOOL EditPortMappingDialog::onCommand(UINT cID, UINT nID)
{
  switch (cID) {
  case IDOK:
    onOkButtonClick();
    break;
  case IDCANCEL:
    onCancelButtonClick();
    break;
  }
  return TRUE;
}
