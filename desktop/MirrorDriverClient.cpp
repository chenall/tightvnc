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

#include "MirrorDriverClient.h"
#include "util/Exception.h"
#include "win-system/Environment.h"
// FIXME: Why the class should depence from the tvnserver-app project?
#include "tvnserver-app/NamingDefs.h"

const TCHAR MirrorDriverClient::MINIPORT_REGISTRY_PATH[] =
  _T("SYSTEM\\CurrentControlSet\\Hardware Profiles\\")
  _T("Current\\System\\CurrentControlSet\\Services");

MirrorDriverClient::MirrorDriverClient(LogWriter *log)
: m_isDriverOpened(false),
  m_isDriverLoaded(false),
  m_isDriverAttached(false),
  m_isDriverConnected(false),
  m_isDisplayChanged(false),
  m_deviceNumber(0),
  m_driverDC(0),
  m_changesBuffer(0),
  m_screenBuffer(0),
  m_propertyChangeListenerWindow(GetModuleHandle(0),
    NamingDefs::MIRROR_DRIVER_MESSAGE_WINDOW_CLASS_NAME),
  m_log(log)
{
  memset(&m_deviceMode, 0, sizeof(m_deviceMode));
  m_deviceMode.dmSize = sizeof(DEVMODE);

  open();
  load();
  connect();

  resume();
  m_initListener.waitForEvent();
  if (m_propertyChangeListenerWindow.getHWND() == 0) {
    dispose();
    throw Exception(_T("Can't create a client for a mirror driver because")
                    _T(" can't create a message window to listen changing")
                    _T(" screen properties."));
  }
}

MirrorDriverClient::~MirrorDriverClient()
{
  try {
    terminate();
    wait();

    dispose();
  } catch (Exception &e) {
    m_log->error(_T("An error occured during the")
               _T(" mirror driver deinitialization: %s"), e.getMessage());
  }
}

void MirrorDriverClient::dispose()
{
  if (m_isDriverConnected) {
    disconnect();
  }
  if (m_isDriverLoaded) {
    unload();
  }
  if (m_isDriverOpened) {
    close();
  }
}

PixelFormat MirrorDriverClient::getPixelFormat() const
{
  return m_pixelFormat;
}

Dimension MirrorDriverClient::getDimension() const
{
  return m_dimension;
}

void *MirrorDriverClient::getBuffer()
{
  return m_screenBuffer;
}

CHANGES_BUF *MirrorDriverClient::getChangesBuf() const
{
  return m_changesBuffer;
}

bool MirrorDriverClient::getPropertiesChanged()
{
  return m_isDisplayChanged;
}

bool MirrorDriverClient::getScreenSizeChanged()
{
  return m_isDisplayChanged;
}

bool MirrorDriverClient::applyNewProperties()
{
  try {
    m_log->debug(_T("Disposing the mirror driver to apply new properties"));
    dispose();

    m_isDisplayChanged = false;

    m_log->debug(_T("Try load new mirror driver to apply new properties"));
    open();
    load();
    connect();
  } catch (Exception &e) {
    m_log->error(_T("Can't apply new screen properties for the mirror driver:")
               _T(" %s"), e.getMessage());
    return false;
  }
  return true;
}

void MirrorDriverClient::open()
{
  _ASSERT(!m_isDriverOpened);

  extractDeviceInfo(_T("Mirage Driver"));
  openDeviceRegKey(_T("dfmirage"));

  m_isDriverOpened = true;
}

void MirrorDriverClient::extractDeviceInfo(TCHAR *driverName)
{
  memset(&m_deviceInfo, 0, sizeof(m_deviceInfo));
  m_deviceInfo.cb = sizeof(m_deviceInfo);

  m_log->info(_T("Searching for %s ..."), driverName);

  m_deviceNumber = 0;
  BOOL result;
  while (result = EnumDisplayDevices(0, m_deviceNumber, &m_deviceInfo, 0)) {
    m_log->debug(_T("Found: %s"), m_deviceInfo.DeviceString);
    m_log->debug(_T("RegKey: %s"), m_deviceInfo.DeviceKey);
    StringStorage deviceString(m_deviceInfo.DeviceString);
    if (deviceString.isEqualTo(driverName)) {
      m_log->info(_T("%s is found"), driverName);
      break;
    }
    m_deviceNumber++;
  }
  if (!result) {
    StringStorage errMess;
    errMess.format(_T("Can't find %s!"), driverName);
    throw Exception(errMess.getString());
  }
}

void MirrorDriverClient::openDeviceRegKey(TCHAR *miniportName)
{
  StringStorage deviceKey(m_deviceInfo.DeviceKey);
  deviceKey.toUpperCase();
  TCHAR *substrPos = deviceKey.find(_T("\\DEVICE"));
  StringStorage subKey(_T("DEVICE0"));
  if (substrPos != 0) {
    StringStorage str(substrPos);
    if (str.getLength() >= 8) {
      str.getSubstring(&subKey, 1, 7);
    }
  }

  m_log->debug(_T("Opening registry key %s\\%s\\%s"),
             MINIPORT_REGISTRY_PATH,
             miniportName,
             subKey.getString());

  RegistryKey regKeyServices(HKEY_LOCAL_MACHINE, MINIPORT_REGISTRY_PATH, true);
  RegistryKey regKeyDriver(&regKeyServices, miniportName, true);
  m_regkeyDevice.open(&regKeyDriver, subKey.getString(), true);
  if (!regKeyServices.isOpened() || !regKeyDriver.isOpened() ||
      !m_regkeyDevice.isOpened()) {
    throw Exception(_T("Can't open registry for the mirror driver"));
  }
}

void MirrorDriverClient::close()
{
  m_regkeyDevice.close();
  m_isDriverOpened = false;
}

void MirrorDriverClient::load()
{
  _ASSERT(m_isDriverOpened);
  if (!m_isDriverLoaded) {
    m_log->info(_T("Loading mirror driver..."));

    initScreenPropertiesByCurrent();

    WORD drvExtraSaved = m_deviceMode.dmDriverExtra;
    // IMPORTANT: we dont touch extension data and size
    memset(&m_deviceMode, 0, sizeof(DEVMODE));
    // m_deviceMode.dmSize = sizeof(m_deviceMode);
    m_deviceMode.dmSize = sizeof(DEVMODE);
    // 2005.10.07
    m_deviceMode.dmDriverExtra = drvExtraSaved;

    m_deviceMode.dmPelsWidth = m_dimension.width;
    m_deviceMode.dmPelsHeight = m_dimension.height;
    m_deviceMode.dmBitsPerPel = m_pixelFormat.bitsPerPixel;
    m_deviceMode.dmPosition.x = m_leftTopCorner.x;
    m_deviceMode.dmPosition.y = m_leftTopCorner.y;

    m_deviceMode.dmFields = DM_BITSPERPEL | DM_PELSWIDTH |
                            DM_PELSHEIGHT | DM_POSITION;
    m_deviceMode.dmDeviceName[0] = '\0';

    setAttachToDesktop(true);
    commitDisplayChanges(&m_deviceMode);

    // Win 2000 version:
    // m_driverDC = CreateDC(_T("DISPLAY"), m_deviceInfo.DeviceName, NULL, NULL);
    m_driverDC = CreateDC(m_deviceInfo.DeviceName, 0, 0, 0);
    if (!m_driverDC) {
      throw Exception(_T("Can't create device context on mirror driver"));
    }
    m_log->info(_T("Device context is created"));

    m_isDriverLoaded = true;
    m_log->info(_T("Mirror driver is now loaded"));
  }
}

void MirrorDriverClient::initScreenPropertiesByCurrent()
{
  // It is indifferently to cpu usage for the driver what
  // a pixel format will use.
  // So, a fixed pixel format looks good.
  m_pixelFormat.initBigEndianByNative();
  m_pixelFormat.bitsPerPixel = 32;
  m_pixelFormat.redMax = 255;
  m_pixelFormat.redShift = 16;
  m_pixelFormat.greenMax = 255;
  m_pixelFormat.greenShift = 8;
  m_pixelFormat.blueMax = 255;
  m_pixelFormat.blueShift = 0;
  m_pixelFormat.colorDepth = 24;

  Rect virtDeskRect = m_screen.getDesktopRect();
  m_dimension.setDim(&virtDeskRect);
  m_leftTopCorner.setPoint(virtDeskRect.left, virtDeskRect.top);
}

void MirrorDriverClient::setAttachToDesktop(bool value)
{
  if (!m_regkeyDevice.setValueAsInt32(_T("Attach.ToDesktop"),
      (int)value)) {
    throw Exception(_T("Can't set the Attach.ToDesktop."));
  }
  m_isDriverAttached = value;
}

void MirrorDriverClient::commitDisplayChanges(DEVMODE *pdm)
{
  // MSDN: Passing NULL for the lpDevMode parameter and 0 for the
  // dwFlags parameter is the easiest way to return to the default
  // mode after a dynamic mode change.
  // But the "default mode" does not mean that the driver is
  // turned off. Especially, when a OS was turned off with turned on driver.
  // (The driver is deactivated but a default mode is saved as was in
  // previous session)

  // 2005.05.21
  // PRB: XP does not work with the parameters:
  // ChangeDisplaySettingsEx(m_deviceInfo.DeviceName, pdm, NULL,
  //                         CDS_UPDATEREGISTRY, NULL)
  // And the 2000 does not work with DEVMODE that has the set DM_POSITION bit.
  m_log->info(_T("commitDisplayChanges(1): \"%s\""), m_deviceInfo.DeviceName);
  if (pdm) {
    LONG code = ChangeDisplaySettingsEx(m_deviceInfo.DeviceName, pdm, 0, CDS_UPDATEREGISTRY, 0);
    if (code < 0) {
      StringStorage errMess;
      errMess.format(_T("1st ChangeDisplaySettingsEx() failed with code %d"),
                     (int)code);
      throw Exception(errMess.getString());
    }
    m_log->info(_T("CommitDisplayChanges(2): \"%s\""), m_deviceInfo.DeviceName);
    code = ChangeDisplaySettingsEx(m_deviceInfo.DeviceName, pdm, 0, 0, 0);
    if (code < 0) {
      StringStorage errMess;
      errMess.format(_T("2nd ChangeDisplaySettingsEx() failed with code %d"),
                     (int)code);
      throw Exception(errMess.getString());
    }
  } else {
    LONG code = ChangeDisplaySettingsEx(m_deviceInfo.DeviceName, 0, 0, 0, 0);
    if (code < 0) {
      StringStorage errMess;
      errMess.format(_T("ChangeDisplaySettingsEx() failed with code %d"),
                     (int)code);
      throw Exception(errMess.getString());
    }
  }
  m_log->info(_T("ChangeDisplaySettingsEx() was successfull"));
}

void MirrorDriverClient::unload()
{
  if (m_driverDC != 0) {
    DeleteDC(m_driverDC);
    m_driverDC = 0;
    m_log->info(_T("The mirror driver device context released"));
  }

  if (m_isDriverAttached) {
    m_log->info(_T("Unloading mirror driver..."));

    setAttachToDesktop(false);

    m_deviceMode.dmPelsWidth = 0;
    m_deviceMode.dmPelsHeight = 0;

    // IMPORTANT: Windows 2000 fails to unload the driver
    // if the mode passed to ChangeDisplaySettingsEx() contains DM_POSITION set.
    DEVMODE *pdm = 0;
    if (!Environment::isWin2000()) {
      pdm = &m_deviceMode;
    }

    try {
      commitDisplayChanges(pdm);
      m_log->info(_T("Mirror driver is unloaded"));
    } catch (Exception &e) {
      m_log->warning(_T("Failed to unload the mirror driver: %s"),
                   e.getMessage());
    }
  }

  // NOTE: extension data and size is also reset
  memset(&m_deviceMode, 0, sizeof(m_deviceMode));
  m_deviceMode.dmSize = sizeof(DEVMODE);

  m_isDriverLoaded = false;
}

void MirrorDriverClient::connect()
{
  m_log->info(_T("Try to connect to the mirror driver."));
  if (!m_isDriverConnected) {
    GETCHANGESBUF buf = {0};
    int res = ExtEscape(m_driverDC, dmf_esc_usm_pipe_map, 0, 0, sizeof(buf), (LPSTR)&buf);
    if (res <= 0) {
      StringStorage errMess;
      errMess.format(_T("Can't set a connection for the mirror driver: ")
                     _T("ExtEscape() failed with %d"),
                     res);
      throw Exception(errMess.getString());
    }

    m_changesBuffer = buf.buffer;
    m_screenBuffer = buf.Userbuffer;

    m_isDriverConnected = true;
  }
}

void MirrorDriverClient::disconnect()
{
  m_log->info(_T("Try to disconnect the mirror driver."));
  if (m_isDriverConnected) {
    GETCHANGESBUF buf;
    buf.buffer = m_changesBuffer;
    buf.Userbuffer = m_screenBuffer;

    int res = ExtEscape(m_driverDC, dmf_esc_usm_pipe_unmap, sizeof(buf), (LPSTR)&buf, 0, 0);
    if (res <= 0) {
      m_log->error(_T("Can't unmap buffer: error code = %d"), res);
    }
    m_isDriverConnected = false;
  }
}

bool MirrorDriverClient::processMessage(UINT message,
                                        WPARAM wParam,
                                        LPARAM lParam)
{
  if (message == WM_DISPLAYCHANGE) {
    m_isDisplayChanged = true;
    m_log->debug(_T("Display changing detecting"));
  }
  return true;
}

void MirrorDriverClient::onTerminate()
{
  PostMessage(m_propertyChangeListenerWindow.getHWND(), WM_QUIT, 0, 0);
}

void MirrorDriverClient::execute()
{
  if (!isTerminating()) {
    m_propertyChangeListenerWindow.createWindow(this);
    m_log->info(_T("Mirror driver client window has been created (hwnd = %d)"),
              (int)m_propertyChangeListenerWindow.getHWND());
  }

  m_initListener.notify();

  MSG msg;
  while (!isTerminating()) {
    if (PeekMessage(&msg, m_propertyChangeListenerWindow.getHWND(),
                    0, 0, PM_REMOVE) != 0) {
      if (msg.message == WM_DISPLAYCHANGE) {
        m_isDisplayChanged = true;
      } else {
        DispatchMessage(&msg);
      }
    } else {
      if (WaitMessage() == 0) {
        m_log->error(_T("Mirror driver client thread has failed"));
        terminate();
      }
    }
    Thread::yield();
  }
}
