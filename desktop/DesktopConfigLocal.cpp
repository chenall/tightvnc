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

#include "DesktopConfigLocal.h"
#include "server-config-lib/Configurator.h"
#include "desktop/WindowsInputBlocker.h"

DesktopConfigLocal::DesktopConfigLocal(LogWriter *log)
{
  m_inputBlocker = new WindowsInputBlocker(log);
}

DesktopConfigLocal::~DesktopConfigLocal()
{
  delete m_inputBlocker;
}

void DesktopConfigLocal::updateByNewSettings()
{
  ServerConfig *srvConf = Configurator::getInstance()->getServerConfig();
  bool hardBlocking = srvConf->isBlockingLocalInput();
  m_inputBlocker->setKeyboardBlocking(hardBlocking);
  m_inputBlocker->setMouseBlocking(hardBlocking);

  bool softBlocking = srvConf->isLocalInputPriorityEnabled();
  unsigned int interval = srvConf->getLocalInputPriorityTimeout() * 1000;
  m_inputBlocker->setSoftKeyboardBlocking(softBlocking, interval);
  m_inputBlocker->setSoftMouseBlocking(softBlocking, interval);
}

bool DesktopConfigLocal::isRemoteInputAllowed()
{
  return m_inputBlocker->isRemoteInputAllowed();
}

DateTime DesktopConfigLocal::getLastInputTime() const
{
  return m_inputBlocker->getLastInputTime();
}

void DesktopConfigLocal::correctLastTime(DateTime newTime)
{
  m_inputBlocker->correctLastTime(newTime);
}
