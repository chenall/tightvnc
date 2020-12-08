// Copyright (C) 2011,2012 GlavSoft LLC.
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

#ifndef __MIRRORDRIVERCLIENT_H__
#define __MIRRORDRIVERCLIENT_H__

#include "UpdateKeeper.h"
#include "UpdateListener.h"
#include "rfb/FrameBuffer.h"
#include "win-system/RegistryKey.h"
#include "DisplayEsc.h"
#include "win-system/Screen.h"
#include "gui/MessageWindow.h"
#include "thread/GuiThread.h"
#include "win-system/WindowsEvent.h"
#include "log-writer/LogWriter.h"

class MirrorDriverClient : private GuiThread, private WindowMessageHandler
{
public:
  MirrorDriverClient(LogWriter *log);
  virtual ~MirrorDriverClient();

  PixelFormat getPixelFormat() const;
  Dimension getDimension() const;

  void *getBuffer();
  CHANGES_BUF *getChangesBuf() const;

  bool getPropertiesChanged();
  bool getScreenSizeChanged();

  bool applyNewProperties();

  void open();
  void close();

  void load();
  void unload();

  void connect();
  void disconnect();

private:
  static const TCHAR MINIPORT_REGISTRY_PATH[];

  static const int EXT_DEVMODE_SIZE_MAX = 3072;
  struct DFEXT_DEVMODE : DEVMODE
  {
    char extension[EXT_DEVMODE_SIZE_MAX];
  };

private:
  virtual bool processMessage(UINT message, WPARAM wParam, LPARAM lParam);

  virtual void execute();
  virtual void onTerminate();

  void dispose();

  void extractDeviceInfo(TCHAR *driverName);
  void openDeviceRegKey(TCHAR *miniportName);

  void initScreenPropertiesByCurrent();
  // value - true to attach, false to detach.
  void setAttachToDesktop(bool value);
  void commitDisplayChanges(DEVMODE *pdm);

  // Driver states.
  bool m_isDriverOpened;
  bool m_isDriverLoaded;
  bool m_isDriverAttached;
  bool m_isDriverConnected;

  DWORD m_deviceNumber;
  DISPLAY_DEVICE m_deviceInfo;
  RegistryKey m_regkeyDevice;
  DFEXT_DEVMODE m_deviceMode;
  HDC m_driverDC;

  CHANGES_BUF *m_changesBuffer;
  void *m_screenBuffer;

  WindowsEvent m_initListener;
  bool m_isDisplayChanged;
  MessageWindow m_propertyChangeListenerWindow;

  PixelFormat m_pixelFormat;
  Dimension m_dimension;
  Point m_leftTopCorner;
  Screen m_screen;

  LogWriter *m_log;
};

#endif // __MIRRORDRIVERCLIENT_H__
