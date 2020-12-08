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

#include "ControlProxy.h"
#include "tvncontrol-app/ControlProto.h"
#include "thread/AutoLock.h"

#include <crtdbg.h>

ControlProxy::ControlProxy(ControlGate *gate)
: m_gate(gate), m_message(0),
  m_getPassFromConfigEnabled(false),
  m_forService(false)
{
}

ControlProxy::~ControlProxy()
{
  releaseMessage();
}

void ControlProxy::setPasswordProperties(const TCHAR *passwordFile,
                                         bool getPassFromConfigEnabled,
                                         bool forService)
{
  m_passwordFile.setString(passwordFile);
  m_getPassFromConfigEnabled = getPassFromConfigEnabled;
  m_forService = forService;
}

TvnServerInfo ControlProxy::getServerInfo()
{
  TvnServerInfo ret;

  AutoLock l(m_gate);

  createMessage(ControlProto::GET_SERVER_INFO_MSG_ID)->send();

  ret.m_acceptFlag = m_gate->readUInt8() == 1;
  ret.m_serviceFlag = m_gate->readUInt8() == 1;

  m_gate->readUTF8(&ret.m_statusText);

  return ret;
}

void ControlProxy::reloadServerConfig()
{
  AutoLock l(m_gate);

  createMessage(ControlProto::RELOAD_CONFIG_MSG_ID)->send();
}

void ControlProxy::disconnectAllClients()
{
  AutoLock l(m_gate);

  createMessage(ControlProto::DISCONNECT_ALL_CLIENTS_MSG_ID)->send();
}

void ControlProxy::shutdownTightVnc()
{
  AutoLock l(m_gate);

  createMessage(ControlProto::SHUTDOWN_SERVER_MSG_ID)->send();
}

void ControlProxy::getClientsList(list<RfbClientInfo *> *clients)
{
  AutoLock l(m_gate);

  createMessage(ControlProto::GET_CLIENT_LIST_MSG_ID)->send();

  UINT32 count = m_gate->readUInt32();

  for (UINT32 i = 0; i < count; i++) {
    StringStorage peerAddr;

    UINT32 id = m_gate->readUInt32();

    m_gate->readUTF8(&peerAddr);

    RfbClientInfo *clientInfo = new RfbClientInfo(id, peerAddr.getString());

    clients->push_back(clientInfo);
  }
}

void ControlProxy::makeOutgoingConnection(const TCHAR *connectString, bool viewOnly)
{
  AutoLock l(m_gate);

  ControlMessage *msg = createMessage(ControlProto::ADD_CLIENT_MSG_ID);

  msg->writeUTF8(connectString);
  msg->writeUInt8(viewOnly);

  msg->send();
}

void ControlProxy::makeTcpDispatcherConnection(const TCHAR *connectString,
                                               const TCHAR *dispatcherName,
                                               const TCHAR *keyword,
                                               UINT32 connectionId)
{
  AutoLock l(m_gate);

  ControlMessage *msg = createMessage(ControlProto::CONNECT_TO_TCPDISP_MSG_ID);

  msg->writeUTF8(connectString);
  msg->writeUTF8(dispatcherName);
  msg->writeUTF8(keyword);
  msg->writeUInt32(connectionId);

  msg->send();
}

void ControlProxy::sharePrimary()
{
  AutoLock l(m_gate);
  ControlMessage *msg = createMessage(ControlProto::SHARE_PRIMARY_MSG_ID);
  msg->send();
}

void ControlProxy::shareDisplay(unsigned char displayNumber)
{
  AutoLock l(m_gate);
  ControlMessage *msg = createMessage(ControlProto::SHARE_DISPLAY_MSG_ID);
  msg->writeUInt8(displayNumber);
  msg->send();
}

void ControlProxy::shareRect(const Rect *shareRect)
{
  AutoLock l(m_gate);
  ControlMessage *msg = createMessage(ControlProto::SHARE_RECT_MSG_ID);

  msg->writeInt32(shareRect->left);
  msg->writeInt32(shareRect->top);
  msg->writeInt32(shareRect->right);
  msg->writeInt32(shareRect->bottom);

  msg->send();
}

void ControlProxy::shareWindow(const StringStorage *shareWindowName)
{
  AutoLock l(m_gate);
  ControlMessage *msg = createMessage(ControlProto::SHARE_WINDOW_MSG_ID);
  msg->writeUTF8(shareWindowName->getString());
  msg->send();
}

void ControlProxy::shareFull()
{
  AutoLock l(m_gate);
  ControlMessage *msg = createMessage(ControlProto::SHARE_FULL_MSG_ID);
  msg->send();
}

void ControlProxy::shareApp(unsigned int procId)
{
  AutoLock l(m_gate);
  ControlMessage *msg = createMessage(ControlProto::SHARE_APP_MSG_ID);
  msg->writeUInt32(procId);
  msg->send();
}

void ControlProxy::setServerConfig(ServerConfig *config)
{
  AutoLock l(m_gate);

  ControlMessage *msg = createMessage(ControlProto::SET_CONFIG_MSG_ID);

  config->serialize(msg);

  msg->send();
}

void ControlProxy::getServerConfig(ServerConfig *config)
{
  AutoLock l(m_gate);

  createMessage(ControlProto::GET_CONFIG_MSG_ID)->send();

  config->deserialize(m_gate);
}

bool ControlProxy::getShowTrayIconFlag()
{
  AutoLock l(m_gate);

  createMessage(ControlProto::GET_SHOW_TRAY_ICON_FLAG)->send();

  return m_gate->readUInt8() == 1;
}

void ControlProxy::updateTvnControlProcessId(DWORD processId)
{
  AutoLock l(m_gate);

  ControlMessage *msg = createMessage(ControlProto::UPDATE_TVNCONTROL_PROCESS_ID_MSG_ID);

  msg->writeUInt32(processId);

  msg->send();
}

ControlMessage *ControlProxy::createMessage(DWORD messageId)
{
  releaseMessage();

  m_message = new ControlMessage(messageId, m_gate, m_passwordFile.getString(),
                                 m_getPassFromConfigEnabled, m_forService);

  return m_message;
}

void ControlProxy::releaseMessage()
{
  if (m_message != 0) {
    delete m_message;

    m_message = 0;
  }
}
