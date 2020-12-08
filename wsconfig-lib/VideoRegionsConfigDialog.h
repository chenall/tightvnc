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

#ifndef _VIDEO_REGIONS_CONFIG_DIALOG_H_
#define _VIDEO_REGIONS_CONFIG_DIALOG_H_

#include "server-config-lib/Configurator.h"

#include "gui/BaseDialog.h"
#include "gui/TextBox.h"
#include "gui/SpinControl.h"

class VideoRegionsConfigDialog : public BaseDialog
{
public:
  VideoRegionsConfigDialog();
  virtual ~VideoRegionsConfigDialog();

  void setParentDialog(BaseDialog *dialog);

  //
  // BaseDialog overrided methods
  //

  virtual BOOL onInitDialog();
  virtual BOOL onNotify(UINT controlID, LPARAM data);
  virtual BOOL onCommand(UINT controlID, UINT notificationID);
  virtual BOOL onDestroy() { return TRUE; }

  //
  // Helper methods
  //

  bool validateInput();
  void updateUI();
  void apply();

protected:

  void initControls();

  //
  // Control event handlers
  //

  void onRecognitionIntervalSpinChangePos(LPNMUPDOWN message);
  void onRecognitionIntervalUpdate();
  void onVideoRegionsUpdate();

protected:
  ServerConfig *m_config;
  TextBox m_videoClasses;
  TextBox m_videoRects;
  TextBox m_videoRecognitionInterval;
  SpinControl m_videoRecognitionIntervalSpin;
  BaseDialog *m_parentDialog;
};

#endif
