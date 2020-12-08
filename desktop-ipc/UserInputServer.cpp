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

#include "UserInputServer.h"
#include "thread/AutoLock.h"
#include "util/BrokenHandleException.h"

UserInputServer::UserInputServer(BlockingGate *forwGate,
                                 DesktopSrvDispatcher *dispatcher,
                                 AnEventListener *extTerminationListener,
                                 LogWriter *log)
: DesktopServerProto(forwGate),
  m_extTerminationListener(extTerminationListener),
  m_log(log)
{
  bool ctrlAltDelEnabled = true;
  m_userInput = new WindowsUserInput(this, ctrlAltDelEnabled, m_log);

  dispatcher->registerNewHandle(POINTER_POS_CHANGED, this);
  dispatcher->registerNewHandle(CLIPBOARD_CHANGED, this);
  dispatcher->registerNewHandle(KEYBOARD_EVENT, this);
  dispatcher->registerNewHandle(USER_INFO_REQ, this);
  dispatcher->registerNewHandle(DESKTOP_COORDS_REQ, this);
  dispatcher->registerNewHandle(WINDOW_COORDS_REQ, this);
  dispatcher->registerNewHandle(WINDOW_HANDLE_REQ, this);
  dispatcher->registerNewHandle(DISPLAY_NUMBER_COORDS_REQ, this);
  dispatcher->registerNewHandle(APPLICATION_REGION_REQ, this);
  dispatcher->registerNewHandle(APPLICATION_CHECK_FOCUS, this);
  dispatcher->registerNewHandle(NORMALIZE_RECT_REQ, this);
  dispatcher->registerNewHandle(USER_INPUT_INIT, this);
}

UserInputServer::~UserInputServer()
{
  m_log->debug(_T("The UserInputServer destructor has been called"));
  delete m_userInput;
}

void UserInputServer::onClipboardUpdate(const StringStorage *newClipboard)
{
  AutoLock al(m_forwGate);
  try {
    // Send clipboard data
    if (newClipboard->getLength() != 0) {
      m_forwGate->writeUInt8(CLIPBOARD_CHANGED);
      sendNewClipboard(newClipboard, m_forwGate);
    }
  } catch (Exception &e) {
    m_log->error(_T("An error has been occurred while sending a")
               _T(" CLIPBOARD_CHANGED message from UserInputServer: %s"),
               e.getMessage());
    m_extTerminationListener->onAnObjectEvent();
  }
}

void UserInputServer::onRequest(UINT8 reqCode, BlockingGate *backGate)
{
  switch (reqCode) {
  case POINTER_POS_CHANGED:
    applyNewPointerPos(backGate);
    break;
  case CLIPBOARD_CHANGED:
    applyNewClipboard(backGate);
    break;
  case KEYBOARD_EVENT:
    applyKeyEvent(backGate);
    break;
  case USER_INFO_REQ:
    ansUserInfo(backGate);
    break;
  case DESKTOP_COORDS_REQ:
    ansDesktopCoords(backGate);
    break;
  case WINDOW_COORDS_REQ:
    ansWindowCoords(backGate);
    break;
  case WINDOW_HANDLE_REQ:
    ansWindowHandle(backGate);
    break;
  case DISPLAY_NUMBER_COORDS_REQ:
    ansDisplayNumberCoords(backGate);
    break;
  case APPLICATION_REGION_REQ:
    ansApplicationRegion(backGate);
    break;
  case APPLICATION_CHECK_FOCUS:
    ansApplicationInFocus(backGate);
    break;
  case NORMALIZE_RECT_REQ:
    ansNormalizeRect(backGate);
    break;
  case USER_INPUT_INIT:
    serverInit(backGate);
    break;
  default:
    StringStorage errMess;
    errMess.format(_T("Unknown %d protocol code received")
                   _T(" from a UserInputClient"), reqCode);
    throw Exception(errMess.getString());
    break;
  }
}

void UserInputServer::serverInit(BlockingGate *backGate)
{
  UINT8 keyFlags = backGate->readUInt8();
  m_userInput->initKeyFlag(keyFlags);
}

void UserInputServer::applyNewPointerPos(BlockingGate *backGate)
{
  Point newPointerPos;
  UINT8 keyFlags;
  readNewPointerPos(&newPointerPos, &keyFlags, backGate);
  m_userInput->setMouseEvent(&newPointerPos, keyFlags);
}

void UserInputServer::applyNewClipboard(BlockingGate *backGate)
{
  StringStorage newClipboard;
  readNewClipboard(&newClipboard, backGate);
  m_userInput->setNewClipboard(&newClipboard);
}

void UserInputServer::applyKeyEvent(BlockingGate *backGate)
{
  UINT32 keySym;
  bool down;
  readKeyEvent(&keySym, &down, backGate);
  m_userInput->setKeyboardEvent(keySym, down);
}

void UserInputServer::ansUserInfo(BlockingGate *backGate)
{
  StringStorage desktopName, userName;

  m_userInput->getCurrentUserInfo(&desktopName, &userName);
  sendUserInfo(&desktopName, &userName, backGate);
}

void UserInputServer::ansDesktopCoords(BlockingGate *backGate)
{
  Rect rect;
  m_userInput->getPrimaryDisplayCoords(&rect);
  sendRect(&rect, backGate);
}

void UserInputServer::ansWindowCoords(BlockingGate *backGate)
{
  Rect rect;
  HWND hwnd = (HWND)backGate->readUInt64();
  try {
    m_userInput->getWindowCoords(hwnd, &rect);
    // handle is correct
    backGate->writeUInt8(0);
    sendRect(&rect, backGate);
  } catch (BrokenHandleException &e) {
    backGate->writeUInt8(1);
    backGate->writeUTF8(e.getMessage());
  }
}

void UserInputServer::ansWindowHandle(BlockingGate *backGate)
{
  StringStorage windowName;
  backGate->readUTF8(&windowName);
  HWND hwnd = m_userInput->getWindowHandleByName(&windowName);
  backGate->writeUInt64((UINT64)hwnd);
}

void UserInputServer::ansDisplayNumberCoords(BlockingGate *backGate)
{
  unsigned char dispNumber = backGate->readUInt8();
  Rect rect;
  m_userInput->getDisplayNumberCoords(&rect, dispNumber);
  sendRect(&rect, backGate);
}

void UserInputServer::ansNormalizeRect(BlockingGate *backGate)
{
  Rect rect = readRect(backGate);
  m_userInput->getNormalizedRect(&rect);
  sendRect(&rect, backGate);
}

void UserInputServer::ansApplicationRegion(BlockingGate *backGate)
{
  UINT32 procId = backGate->readUInt32();
  Region region;
  m_userInput->getApplicationRegion(procId, &region);
  sendRegion(&region, backGate);
}

void UserInputServer::ansApplicationInFocus(BlockingGate *backGate)
{
  UINT32 procId = backGate->readUInt32();
  bool result = m_userInput->isApplicationInFocus((unsigned int)procId);
  backGate->writeUInt8(result ? 1 : 0);
}
