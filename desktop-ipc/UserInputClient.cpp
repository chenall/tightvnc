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

#include "UserInputClient.h"
#include "thread/AutoLock.h"
#include "ReconnectException.h"
#include "util/BrokenHandleException.h"

UserInputClient::UserInputClient(BlockingGate *forwGate,
                                 DesktopSrvDispatcher *dispatcher,
                                 ClipboardListener *clipboardListener)
: DesktopServerProto(forwGate),
  m_clipboardListener(clipboardListener),
  m_sendMouseFlags(0)
{
  dispatcher->registerNewHandle(CLIPBOARD_CHANGED, this);
}

UserInputClient::~UserInputClient()
{
}

void UserInputClient::onRequest(UINT8 reqCode, BlockingGate *backGate)
{
  switch (reqCode) {
  case CLIPBOARD_CHANGED:
    {
      StringStorage newClipboard;
      readNewClipboard(&newClipboard, backGate);
      m_clipboardListener->onClipboardUpdate(&newClipboard);
    }
    break;
  default:
    StringStorage errMess;
    errMess.format(_T("Unknown %d protocol code received from a pipe ")
                   _T("UserInputServer"), (int)reqCode);
    throw Exception(errMess.getString());
    break;
  }
}

void UserInputClient::sendInit(BlockingGate *gate)
{
  AutoLock al(gate);
  gate->writeUInt8(USER_INPUT_INIT);
  gate->writeUInt8(m_sendMouseFlags);
}

void UserInputClient::setMouseEvent(const Point *newPos, UINT8 keyFlag)
{
  AutoLock al(m_forwGate);
  try {
    // Send mouse data
    m_forwGate->writeUInt8(POINTER_POS_CHANGED);
    sendNewPointerPos(newPos, keyFlag, m_forwGate);
    m_sendMouseFlags = keyFlag;
  } catch (ReconnectException &) {
  }
}

void UserInputClient::setNewClipboard(const StringStorage *newClipboard)
{
  AutoLock al(m_forwGate);
  try {
    // Send clipboard data
    m_forwGate->writeUInt8(CLIPBOARD_CHANGED);
    sendNewClipboard(newClipboard, m_forwGate);
  } catch (ReconnectException &) {
  }
}

void UserInputClient::setKeyboardEvent(UINT32 keySym, bool down)
{
  AutoLock al(m_forwGate);
  try {
    // Send keyboard data
    m_forwGate->writeUInt8(KEYBOARD_EVENT);
    sendKeyEvent(keySym, down, m_forwGate);
  } catch (ReconnectException &) {
  }
}

void UserInputClient::getCurrentUserInfo(StringStorage *desktopName,
                                         StringStorage *userName)
{
  AutoLock al(m_forwGate);
  try {
    // Send request
    m_forwGate->writeUInt8(USER_INFO_REQ);
    readUserInfo(desktopName, userName, m_forwGate);
  } catch (ReconnectException &) {
  }
}

void UserInputClient::getPrimaryDisplayCoords(Rect *rect)
{
  AutoLock al(m_forwGate);
  bool success = false;
  do {
    try {
      // Send request
      m_forwGate->writeUInt8(DESKTOP_COORDS_REQ);
      *rect = readRect(m_forwGate);
      success = true;
    } catch (ReconnectException &) {
    }
  } while (!success);
}

void UserInputClient::getDisplayNumberCoords(Rect *rect,
                                             unsigned char dispNumber)
{
  AutoLock al(m_forwGate);
  bool success = false;
  do {
    try {
      // Send request
      m_forwGate->writeUInt8(DISPLAY_NUMBER_COORDS_REQ);
      m_forwGate->writeUInt8(dispNumber);
      *rect = readRect(m_forwGate);
      success = true;
    } catch (ReconnectException &) {
    }
  } while (!success);
}

void UserInputClient::getNormalizedRect(Rect *rect)
{
  AutoLock al(m_forwGate);
  bool success = false;
  do {
    try {
      // Send request
      m_forwGate->writeUInt8(NORMALIZE_RECT_REQ);
      sendRect(rect, m_forwGate);
      *rect = readRect(m_forwGate);
      success = true;
    } catch (ReconnectException &) {
    }
  } while (!success);
}

void UserInputClient::getWindowCoords(HWND hwnd, Rect *rect)
{
  AutoLock al(m_forwGate);
  bool success = false;
  do {
    try {
      // Send request
      m_forwGate->writeUInt8(WINDOW_COORDS_REQ);
      m_forwGate->writeUInt64((UINT64)hwnd);
      bool isBrokenWindow = m_forwGate->readUInt8() != 0;
      if (!isBrokenWindow) {
        *rect = readRect(m_forwGate);
      } else {
        // Receive error discription (do not generate it here).
        // This made to avoid code duplication.
        StringStorage errMess;
        m_forwGate->readUTF8(&errMess);
        throw BrokenHandleException(errMess.getString());
      }
      success = true;
    } catch (ReconnectException &) {
    }
  } while (!success);
}

HWND UserInputClient::getWindowHandleByName(const StringStorage *windowName)
{
  AutoLock al(m_forwGate);
  bool success = false;
  HWND hwnd = 0;
  do {
    try {
      // Send request
      m_forwGate->writeUInt8(WINDOW_HANDLE_REQ);
      m_forwGate->writeUTF8(windowName->getString());
      hwnd = (HWND)m_forwGate->readUInt64();
      success = true;
    } catch (ReconnectException &) {
    }
  } while (!success);
  return hwnd;
}

void UserInputClient::getApplicationRegion(unsigned int procId, Region *region)
{
  AutoLock al(m_forwGate);
  bool success = false;
  do {
    try {
      // Send request
      m_forwGate->writeUInt8(APPLICATION_REGION_REQ);
      m_forwGate->writeUInt32(procId);
      readRegion(region, m_forwGate);
      success = true;
    } catch (ReconnectException &) {
    }
  } while (!success);
}

bool UserInputClient::isApplicationInFocus(unsigned int procId)
{
  bool result = false;

  AutoLock al(m_forwGate);
  bool success = false;
  do {
    try {
      // Send request
      m_forwGate->writeUInt8(APPLICATION_CHECK_FOCUS);
      m_forwGate->writeUInt32(procId);
      // readRegion(region, m_forwGate);
      result = (m_forwGate->readUInt8() != 0);
      success = true;
    } catch (ReconnectException &) {
    }
  } while (!success);

  return result;
}
