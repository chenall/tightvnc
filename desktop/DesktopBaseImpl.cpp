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

#include "DesktopBaseImpl.h"
#include "util/BrokenHandleException.h"

DesktopBaseImpl::DesktopBaseImpl(ClipboardListener *extClipListener,
                       UpdateSendingListener *extUpdSendingListener,
                       AbnormDeskTermListener *extDeskTermListener,
                       LogWriter *log)
: m_extUpdSendingListener(extUpdSendingListener),
  m_extDeskTermListener(extDeskTermListener),
  m_extClipListener(extClipListener),
  m_userInput(0),
  m_updateHandler(0),
  m_log(log)
{
}

DesktopBaseImpl::~DesktopBaseImpl()
{
}

void DesktopBaseImpl::getCurrentUserInfo(StringStorage *desktopName,
                                    StringStorage *userName)
{
  _ASSERT(m_userInput != 0);
  _ASSERT(m_extDeskTermListener != 0);
  m_log->info(_T("get current user information"));
  try {
    m_userInput->getCurrentUserInfo(desktopName, userName);
  } catch (Exception &e) {
	m_log->error(_T("Exception in DesktopBaseImpl::getCurrentUserInfo: %s"), e.getMessage());
    m_extDeskTermListener->onAbnormalDesktopTerminate();
  }
}

void DesktopBaseImpl::getFrameBufferProperties(Dimension *dim, PixelFormat *pf)
{
  _ASSERT(m_updateHandler != 0);
  _ASSERT(m_extDeskTermListener != 0);
  m_log->debug(_T("get frame buffer properties"));
  try {
    m_updateHandler->getFrameBufferProp(dim, pf);
  } catch (Exception &e) {
	m_log->error(_T("Exception in DesktopBaseImpl::getFrameBufferProperties: %s"), e.getMessage());
    m_extDeskTermListener->onAbnormalDesktopTerminate();
  }
}

void DesktopBaseImpl::getPrimaryDesktopCoords(Rect *rect)
{
  _ASSERT(m_userInput != 0);
  _ASSERT(m_extDeskTermListener != 0);
  m_log->info(_T("get primary desktop coordinates"));
  try {
    m_userInput->getPrimaryDisplayCoords(rect);
  } catch (Exception &e) {
	m_log->error(_T("Exception in DesktopBaseImpl::getPrimaryDesktopCoords: %s"), e.getMessage());
    m_extDeskTermListener->onAbnormalDesktopTerminate();
  }
}

void DesktopBaseImpl::getDisplayNumberCoords(Rect *rect,
                                        unsigned char dispNumber)
{
  _ASSERT(m_userInput != 0);
  _ASSERT(m_extDeskTermListener != 0);
  m_log->info(_T("get the %u display coordinates"), (unsigned int) dispNumber);
  try {
    m_userInput->getDisplayNumberCoords(rect, dispNumber);
  } catch (Exception &e) {
	m_log->error(_T("Exception in DesktopBaseImpl::getDisplayNumberCoords: %s"), e.getMessage());
    m_extDeskTermListener->onAbnormalDesktopTerminate();
  }
}

void DesktopBaseImpl::getNormalizedRect(Rect *rect)
{
  _ASSERT(m_userInput != 0);
  _ASSERT(m_extDeskTermListener != 0);
  m_log->info(_T("normilize a rect to frame buffer coordinates"));
  try {
    m_userInput->getNormalizedRect(rect);
  } catch (Exception &e) {
	m_log->error(_T("Exception in DesktopBaseImpl::getNormalizedRect: %s"), e.getMessage());
    m_extDeskTermListener->onAbnormalDesktopTerminate();
  }
}

void DesktopBaseImpl::getWindowCoords(HWND hwnd, Rect *rect)
{
  _ASSERT(m_userInput != 0);
  _ASSERT(m_extDeskTermListener != 0);
  m_log->info(_T("get window coordinates"));
  try {
    m_userInput->getWindowCoords(hwnd, rect);
  } catch (BrokenHandleException &) {
    throw;
  } catch (Exception &e) {
	m_log->error(_T("Exception in DesktopBaseImpl::getWindowCoords: %s"), e.getMessage());
    m_extDeskTermListener->onAbnormalDesktopTerminate();
  }
}

HWND DesktopBaseImpl::getWindowHandleByName(const StringStorage *windowName)
{
  _ASSERT(m_userInput != 0);
  _ASSERT(m_extDeskTermListener != 0);
  m_log->info(_T("get a window handle by a window name"));
  try {
    return m_userInput->getWindowHandleByName(windowName);
  } catch (Exception &e) {
	m_log->error(_T("Exception in DesktopBaseImpl::getWindowHandleByName: %s"), e.getMessage());
    m_extDeskTermListener->onAbnormalDesktopTerminate();
  }
  return 0;
}

void DesktopBaseImpl::getApplicationRegion(unsigned int procId, Region *region)
{
  _ASSERT(m_userInput != 0);
  _ASSERT(m_extDeskTermListener != 0);
  m_log->info(_T("get application region"));
  try {
    m_userInput->getApplicationRegion(procId, region);
  } catch (Exception &e) {
	m_log->error(_T("Exception in DesktopBaseImpl::getApplicationRegion: %s"), e.getMessage());
    m_extDeskTermListener->onAbnormalDesktopTerminate();
  }
}

bool DesktopBaseImpl::isApplicationInFocus(unsigned int procId)
{
  _ASSERT(m_userInput != 0);
  _ASSERT(m_extDeskTermListener != 0);
  m_log->info(_T("checking if application is in focus"));
  try {
    return m_userInput->isApplicationInFocus(procId);
  } catch (Exception &e) {
	m_log->error(_T("Exception in DesktopBaseImpl::isApplicationInFocus %s"), e.getMessage());
    m_extDeskTermListener->onAbnormalDesktopTerminate();
  }
  return false;
}

bool DesktopBaseImpl::isRemoteInputAllowed()
{
  m_log->debug(_T("checking remote input allowing"));

  bool enabled = !Configurator::getInstance()->getServerConfig()->isBlockingRemoteInput();
  enabled = enabled && !isRemoteInputTempBlocked();
  return enabled;
}

void DesktopBaseImpl::setKeyboardEvent(UINT32 keySym, bool down)
{
  _ASSERT(m_userInput != 0);
  _ASSERT(m_extDeskTermListener != 0);

  m_log->info(_T("set keyboard event (keySym = %u, down = %d)"), keySym, (int)down);
  try {
    if (isRemoteInputAllowed()) {
      m_userInput->setKeyboardEvent(keySym, down);
    }
  } catch (Exception &e) {
    m_log->error(_T("setKeyboardEvent() crashed: %s"), e.getMessage());
    m_extDeskTermListener->onAbnormalDesktopTerminate();
  }
}

void DesktopBaseImpl::setMouseEvent(UINT16 x, UINT16 y, UINT8 buttonMask)
{
  _ASSERT(m_userInput != 0);
  _ASSERT(m_extDeskTermListener != 0);

  m_log->info(_T("set mouse event (x = %u, y = %u, mask = %u)"), (UINT32)x, (UINT32)y, (UINT32)buttonMask);
  Point point(x, y);
  try {
    if (isRemoteInputAllowed()) {
      m_userInput->setMouseEvent(&point, buttonMask);
    }
  } catch (Exception &e) {
	m_log->error(_T("Exception in DesktopBaseImpl::setMouseEvent %s"), e.getMessage());
    m_extDeskTermListener->onAbnormalDesktopTerminate();
  }
}

void DesktopBaseImpl::setNewClipText(const StringStorage *newClipboard)
{
  _ASSERT(m_userInput != 0);
  _ASSERT(m_extDeskTermListener != 0);

  m_log->debug(_T("set new clipboard text, length: %d"), newClipboard->getLength());

  {
    AutoLock al(&m_storedClipCritSec);
    m_receivedClip = *newClipboard;
  }
  try {
    m_userInput->setNewClipboard(newClipboard);
  } catch (Exception &e) {
	  m_log->error(_T("Exception in DesktopBaseImpl::setNewClipText %s"), e.getMessage());
    m_extDeskTermListener->onAbnormalDesktopTerminate();
  }
}

void DesktopBaseImpl::sendUpdate()
{
  _ASSERT(m_updateHandler != 0);
  _ASSERT(m_extDeskTermListener != 0);
  _ASSERT(m_extUpdSendingListener != 0);

  if (!m_extUpdSendingListener->isReadyToSend()) {
    m_log->detail(_T("nobody is ready for updates"));
    return;
  }
  UpdateContainer updCont;
  try {
    if (!m_fullReqRegion.isEmpty()) {
      m_log->detail(_T("set full update request to UpdateHandler"));
      m_updateHandler->setFullUpdateRequested(&m_fullReqRegion);
    }

    m_log->detail(_T("extracting updates from UpdateHandler"));
    m_updateHandler->extract(&updCont);
  } catch (Exception &e) {
    m_log->info(_T("WinDesktop::sendUpdate() failed with error:%s"),
               e.getMessage());
    m_extDeskTermListener->onAbnormalDesktopTerminate();
  }

  if (!updCont.isEmpty() || !m_fullReqRegion.isEmpty()) {
    m_log->debug(_T("UpdateContainer is not empty.")
              _T(" Updates will be given to all."));
    m_extUpdSendingListener->onSendUpdate(&updCont,
                                          m_updateHandler->getCursorShape());
    AutoLock al(&m_reqRegMutex);
    m_fullReqRegion.clear();
  } else {
    m_log->info(_T("UpdateContainer is empty"));
  }
}

void DesktopBaseImpl::onUpdate()
{
  m_log->detail(_T("update detected"));
  m_newUpdateEvent.notify();
}

void DesktopBaseImpl::onUpdateRequest(const Rect *rectRequested, bool incremental)
{
  m_log->debug(_T("DesktopBaseImpl::onUpdateRequest: update requested"));

  AutoLock al(&m_reqRegMutex);
  if (!incremental) {
    m_fullReqRegion.addRect(rectRequested);
  }
  m_newUpdateEvent.notify();
}

void DesktopBaseImpl::onClipboardUpdate(const StringStorage *newClipboard)
{
  _ASSERT(m_extClipListener != 0);

  m_log->detail(_T("clipboard update detected, length: %d"), newClipboard->getLength());
  bool isEqual;
  {
    AutoLock al(&m_storedClipCritSec);
    isEqual = m_receivedClip.isEqualTo(newClipboard);
  }
  if (!isEqual) {
    {
      AutoLock al(&m_storedClipCritSec);
      m_receivedClip = _T("");
    }
    // Send new clipboard text, even if it is empty.
    m_log->debug(_T("Send new clipboard content"));
    m_extClipListener->onClipboardUpdate(newClipboard);
  }
  else {
    m_log->debug(_T("do not send new clipboard content"));
  }
}

void DesktopBaseImpl::onConfigReload(ServerConfig *serverConfig)
{
  applyNewConfiguration();
}

bool DesktopBaseImpl::updateExternalFrameBuffer(FrameBuffer *fb, const Region *region,
                                           const Rect *viewPort)
{
  return m_updateHandler->updateExternalFrameBuffer(fb, region, viewPort);
}
