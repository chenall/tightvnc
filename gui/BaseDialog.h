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

#ifndef _BASE_DIALOG_H_
#define _BASE_DIALOG_H_

#include "Control.h"
#include "util/StringStorage.h"

class BaseDialog
{
public:
  BaseDialog();
  BaseDialog(DWORD resourceId);
  BaseDialog(const TCHAR *resourceName);
  virtual ~BaseDialog();
public:

  // Method creates non modal window but not shows it
  void create();

  //
  // Methods creates windows and show it in nonmodal/modal mode
  //

  int show();
  int showModal();

  // Returns true if dialog is already created.
  bool isCreated();

  // Method hides window
  void hide();
  // Method closes dialog
  virtual void kill(int code);
  // Method sets parent window
  void setParent(Control *ctrlParent);
  // Set resource name for dialog
  void setResourceName(const TCHAR *resourceName);
  // Set resource id for dialog.
  void setResourceId(DWORD id);
  // Return
  Control *getControl() { return &m_ctrlThis; }
  // Setup control by ID
  void setControlById(Control &control, DWORD id);
  // Icon manipulation 
  void loadIcon(DWORD id);
  void updateIcon();

  // Puts this control foreground and activates it
  bool setForeground();

protected:
  /**
   * Sets default push button for dialog.
   * @pararm buttonId new default push button id.
   */
  void setDefaultPushButton(UINT buttonId);

protected:

  //
  // This methods must be overrided by child classes.
  //

  virtual BOOL onInitDialog();
  virtual BOOL onNotify(UINT controlID, LPARAM data);
  virtual BOOL onCommand(UINT controlID, UINT notificationID);
  virtual BOOL onDestroy();

  //
  // This methods can be overrided by child classes.
  //

  virtual BOOL onDrawItem(WPARAM controlID, LPDRAWITEMSTRUCT dis);
  virtual void onMessageReceived(UINT uMsg, WPARAM wParam, LPARAM lParam);

  //
  // Window message proccessing method
  //

  static INT_PTR CALLBACK dialogProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
  TCHAR *getResouceName();

protected:

  TCHAR *m_resourceName;        // Name of dialog resource
  DWORD m_resourceId;            // Id of dialog resouce
  Control m_ctrlThis;           // This dialog control
  Control *m_ctrlParent;        // Parent dialog or NULL if no parent

  bool m_isModal;
  bool m_isCreated;

  HICON m_hicon;
};

#endif
