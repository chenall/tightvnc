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

#include "SasUserInput.h"
#include "win-system/Environment.h"

#define XK_MISCELLANY
#include "rfb/keysymdef.h"

SasUserInput::SasUserInput(UserInputClient *client, LogWriter *log)
: m_client(client),
  m_ctrlPressed(false),
  m_altPressed(false),
  m_underVista(false),
  m_log(log)
{
  m_underVista = Environment::isVistaOrLater();
}

SasUserInput::~SasUserInput()
{
}

void SasUserInput::sendInit(BlockingGate *gate)
{
  m_client->sendInit(gate);
}

void SasUserInput::setMouseEvent(const Point *newPos, UINT8 keyFlag)
{
  m_client->setMouseEvent(newPos, keyFlag);
}

void SasUserInput::setNewClipboard(const StringStorage *newClipboard)
{
  m_client->setNewClipboard(newClipboard);
}

void SasUserInput::setKeyboardEvent(UINT32 keySym, bool down)
{
  bool delPressed = false;

  if (m_underVista) {
    switch (keySym) {
    case XK_Alt_L:
    case XK_Alt_R:
      m_altPressed = down;
      break;
    case XK_Control_L:
    case XK_Control_R:
      m_ctrlPressed = down;
      break;
    case XK_Delete:
      delPressed = down;
    }
  }

  if (m_ctrlPressed && m_altPressed && delPressed && m_underVista) {
    Environment::simulateCtrlAltDelUnderVista(m_log);
  } else {
    m_client->setKeyboardEvent(keySym, down);
  }
}

void SasUserInput::getCurrentUserInfo(StringStorage *desktopName,
                                         StringStorage *userName)
{
  m_client->getCurrentUserInfo(desktopName, userName);
}

void SasUserInput::getPrimaryDisplayCoords(Rect *rect)
{
  m_client->getPrimaryDisplayCoords(rect);
}

void SasUserInput::getDisplayNumberCoords(Rect *rect,
                                          unsigned char dispNumber)
{
  m_client->getDisplayNumberCoords(rect, dispNumber);
}

void SasUserInput::getNormalizedRect(Rect *rect)
{
  m_client->getNormalizedRect(rect);
}

void SasUserInput::getWindowCoords(HWND hwnd, Rect *rect)
{
  m_client->getWindowCoords(hwnd, rect);
}

HWND SasUserInput::getWindowHandleByName(const StringStorage *windowName)
{
  return m_client->getWindowHandleByName(windowName);
}

void SasUserInput::getApplicationRegion(unsigned int procId, Region *region)
{
  m_client->getApplicationRegion(procId, region);
}

bool SasUserInput::isApplicationInFocus(unsigned int procId)
{
  return m_client->isApplicationInFocus(procId);
}
