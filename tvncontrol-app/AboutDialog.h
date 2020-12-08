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

#ifndef _ABOUT_DIALOG_H_
#define _ABOUT_DIALOG_H_

#include "gui/BaseDialog.h"

/**
 * TightVNC server about dialog.
 */
class AboutDialog : public BaseDialog
{
public:
  /**
   * Creates dialog.
   */
  AboutDialog();
  /**
   * Destroys dialog.
   */
  virtual ~AboutDialog();

protected:
  /**
   * Called when user press "Close" button.
   */
  void onCloseButtonClick();
  /**
   * Called when user press "Order Technical Support" button.
   */
  void onOrderSupportButtonClock();
  /**
   * Called when user press "Visit Web Site" button.
   */
  void onVisitSiteButtonClick();
  /**
   * Opens url in default browser or shows error message on fail.
   */
  void openUrl(const TCHAR *url);

protected:
  /**
   * Inherited from BaseDialog.
   * Does nothing.
   */
  virtual BOOL onInitDialog();
  /**
   * Inherited from BaseDialog.
   * Does nothing.
   */
  virtual BOOL onNotify(UINT controlID, LPARAM data);
  /**
   * Inherited from BaseDialog.
   * Handles buttons events.
   */
  virtual BOOL onCommand(UINT controlID, UINT notificationID);
  /**
   * Inherited from BaseDialog.
   * Does nothing.
   */
  virtual BOOL onDestroy();
};

#endif
