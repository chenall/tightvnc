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

#include "ConfigServer.h"
#include "server-config-lib/Configurator.h"

ConfigServer::ConfigServer(DesktopSrvDispatcher *dispatcher, LogWriter *log)
: DesktopServerProto(0),
  m_deskConf(log)
{
  dispatcher->registerNewHandle(CONFIG_RELOAD_REQ, this);
  dispatcher->registerNewHandle(SOFT_INPUT_ENABLING_REQ, this);
}

ConfigServer::~ConfigServer()
{
}

void ConfigServer::onRequest(UINT8 reqCode, BlockingGate *backGate)
{
  switch (reqCode) {
  case CONFIG_RELOAD_REQ:
    reloadSettings(backGate);
    break;
  case SOFT_INPUT_ENABLING_REQ:
    answerOnSoftInputEnablingReq(backGate);
    break;
  default:
    StringStorage errMess;
    errMess.format(_T("Unknown %d protocol code received from a pipe client"),
                   (int)reqCode);
    throw Exception(errMess.getString());
    break;
  }
}

void ConfigServer::reloadSettings(BlockingGate *backGate)
{
  readConfigSettings(backGate);
  Configurator::getInstance()->notifyReload();
  m_deskConf.updateByNewSettings();
}

void ConfigServer::answerOnSoftInputEnablingReq(BlockingGate *backGate)
{
  m_deskConf.correctLastTime(DateTime(backGate->readUInt64()));

  bool allowed = m_deskConf.isRemoteInputAllowed();
  backGate->writeUInt8(allowed);
  backGate->writeUInt64(m_deskConf.getLastInputTime().getTime());
}
