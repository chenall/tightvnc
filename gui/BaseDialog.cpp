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

#include "BaseDialog.h"

#include "util/CommonHeader.h"

#include <commctrl.h>
#include <crtdbg.h>

BaseDialog::BaseDialog()
: m_ctrlParent(NULL), m_resourceName(0), m_resourceId(0), m_hicon(0)
{
}

BaseDialog::BaseDialog(DWORD resourceId)
: m_ctrlParent(NULL), m_resourceName(0), m_resourceId(resourceId), m_hicon(0)
{
}

BaseDialog::BaseDialog(const TCHAR *resourceName)
: m_ctrlParent(NULL), m_resourceName(0), m_resourceId(0), m_hicon(0)
{
  setResourceName(resourceName);
}

BaseDialog::~BaseDialog()
{
  if (m_hicon) {
    DeleteObject(m_hicon);
  }
  if (m_resourceName != 0) {
    free(m_resourceName);
  }
}

void BaseDialog::setResourceName(const TCHAR *resourceName)
{
  if (m_resourceName != 0) {
    free(m_resourceName);
  }

  m_resourceName = _tcsdup(resourceName);
}

void BaseDialog::setResourceId(DWORD id)
{
  m_resourceId = id;
}

void BaseDialog::setDefaultPushButton(UINT buttonId)
{
  SendMessage(m_ctrlThis.getWindow(), DM_SETDEFID, buttonId, 0);
}

void BaseDialog::setParent(Control *ctrlParent)
{
  m_ctrlParent = ctrlParent;
}

int BaseDialog::show()
{
  if (m_ctrlThis.getWindow() == NULL) {
    create();
  } else {
    m_ctrlThis.setForeground();
  }
  return 0;
}

void BaseDialog::hide()
{
  m_ctrlThis.setVisible(false);
}

void BaseDialog::kill(int code)
{
  // Destroy dialog
  if (!m_isModal) {
    DestroyWindow(m_ctrlThis.getWindow());
  } else {
    EndDialog(m_ctrlThis.getWindow(), code);
  }
  // We have no valid hwnd, so forse set hwnd to NULL
  m_ctrlThis.setWindow(NULL);
}

void BaseDialog::create()
{
  HWND window, parentWindow = NULL;

  if (m_ctrlParent != NULL) {
    parentWindow = m_ctrlParent->getWindow();
  }

  window = CreateDialogParam(GetModuleHandle(NULL), getResouceName(),
                             parentWindow, dialogProc, (LPARAM)this);

  m_isModal = false;

  _ASSERT(window != NULL);
}

int BaseDialog::showModal()
{
  int result = 0;
  if (m_ctrlThis.getWindow() == NULL) {
    m_isModal = true;
    HWND parentWindow = (m_ctrlParent != NULL) ? m_ctrlParent->getWindow() : NULL;
    result = (int)DialogBoxParam(GetModuleHandle(NULL),
                                 getResouceName(),
                                 parentWindow, dialogProc, (LPARAM)this);
  } else {
    m_ctrlThis.setVisible(true);
    m_ctrlThis.setForeground();
  }

  //
  // TODO: Place error notification here
  //

  if (result == -1) {
  }

  return result;
}

bool BaseDialog::isCreated()
{
  bool isInit = m_ctrlThis.getWindow() != 0;

  if (!isInit) {
    return false;
  }

  return !!IsWindow(m_ctrlThis.getWindow());
}

BOOL BaseDialog::onDrawItem(WPARAM controlID, LPDRAWITEMSTRUCT dis)
{
  return TRUE;
}

void BaseDialog::onMessageReceived(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
}

INT_PTR CALLBACK BaseDialog::dialogProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  BaseDialog *_this;
  BOOL bResult;

  bResult = FALSE;
  if (uMsg == WM_INITDIALOG) {
    _this = (BaseDialog *)lParam;
    SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)_this);
    _this->m_ctrlThis.setWindow(hwnd);
    _this->updateIcon();
  } else {
    _this = (BaseDialog *)GetWindowLongPtr(hwnd, GWLP_USERDATA);
    if (_this == 0) {
      return FALSE;
    }
  }

  _this->onMessageReceived(uMsg, wParam, lParam);

  switch (uMsg) {
  case WM_INITDIALOG:
    bResult = _this->onInitDialog();
    break;
  case WM_NOTIFY:
    bResult = _this->onNotify(LOWORD(wParam), lParam);
    break;
  case WM_COMMAND:
    bResult =_this->onCommand(LOWORD(wParam), HIWORD(wParam));
    break;
  case WM_DESTROY:
    bResult = _this->onDestroy();
    break;
  case WM_DRAWITEM:
    bResult = _this->onDrawItem(wParam, (LPDRAWITEMSTRUCT)lParam);
    break;
  }

  return bResult;
}

TCHAR *BaseDialog::getResouceName()
{
  if (m_resourceId != 0) {
    return MAKEINTRESOURCE(m_resourceId);
  }
  return m_resourceName;
}

void BaseDialog::setControlById(Control &control, DWORD id) 
{
  control = GetDlgItem(m_ctrlThis.getWindow(), id);
}

void BaseDialog::updateIcon()
{
  if (m_hicon) {
    SetClassLongPtr(m_ctrlThis.getWindow(), GCLP_HICON, (LONG_PTR)m_hicon);
  }
}

void BaseDialog::loadIcon(DWORD id)
{
  if (m_hicon) {
    DeleteObject(m_hicon);
  }
  m_hicon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(id));
}

bool BaseDialog::setForeground()
{
  return m_ctrlThis.setForeground();
}

BOOL BaseDialog::onInitDialog()
{
  return FALSE;
}

BOOL BaseDialog::onNotify(UINT controlID, LPARAM data)
{
  return FALSE;
}

BOOL BaseDialog::onCommand(UINT controlID, UINT notificationID)
{
  return FALSE;
}

BOOL BaseDialog::onDestroy()
{
  return FALSE;
}
