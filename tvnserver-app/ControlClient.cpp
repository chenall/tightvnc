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

#include "ControlClient.h"
#include "TvnServer.h"
#include "OutgoingRfbConnectionThread.h"
#include "ConnectToTcpDispatcherThread.h"

#include "tvncontrol-app/ControlProto.h"

#include "network/socket/SocketStream.h"

#include "server-config-lib/Configurator.h"

#include "util/VncPassCrypt.h"

#include "rfb/HostPath.h"

#include "win-system/WTS.h"

#include "tvnserver/resource.h"

#include <time.h>
#include "util/AnsiStringStorage.h"

const UINT32 ControlClient::REQUIRES_AUTH[] = { ControlProto::ADD_CLIENT_MSG_ID,
                                                ControlProto::DISCONNECT_ALL_CLIENTS_MSG_ID,
                                                ControlProto::GET_CONFIG_MSG_ID,
                                                ControlProto::SET_CONFIG_MSG_ID,
                                                ControlProto::SHUTDOWN_SERVER_MSG_ID,
                                                ControlProto::SHARE_PRIMARY_MSG_ID,
                                                ControlProto::SHARE_DISPLAY_MSG_ID,
                                                ControlProto::SHARE_WINDOW_MSG_ID,
                                                ControlProto::SHARE_RECT_MSG_ID,
                                                ControlProto::SHARE_APP_MSG_ID,
                                                ControlProto::SHARE_FULL_MSG_ID,
                                                ControlProto::CONNECT_TO_TCPDISP_MSG_ID };

const UINT32 ControlClient::WITHOUT_AUTH[] = {
  ControlProto::AUTH_MSG_ID,
  ControlProto::RELOAD_CONFIG_MSG_ID,
  ControlProto::GET_SERVER_INFO_MSG_ID,
  ControlProto::GET_CLIENT_LIST_MSG_ID,
  ControlProto::GET_SHOW_TRAY_ICON_FLAG,
  ControlProto::UPDATE_TVNCONTROL_PROCESS_ID_MSG_ID
};

ControlClient::ControlClient(Transport *transport,
                             RfbClientManager *rfbClientManager,
                             ControlAppAuthenticator *authenticator,
                             HANDLE pipeHandle, LogWriter *log)
: m_transport(transport), m_rfbClientManager(rfbClientManager),
  m_authenticator(authenticator),
  m_tcpDispId(0),
  m_pipeHandle(pipeHandle),
  m_authReqMessageId(0),
  m_log(log),
  m_repeatAuthPassed(false)
{
  m_stream = m_transport->getIOStream();

  m_gate = new ControlGate(m_stream);

  m_authPassed = false;
}

ControlClient::~ControlClient()
{
  terminate();
  wait();

  delete m_gate;
  delete m_transport;
}

void ControlClient::execute()
{
  // Client passes authentication by default if server does not uses control authentication.
  if (!Configurator::getInstance()->getServerConfig()->isControlAuthEnabled()) {
    m_authPassed = true;
  }

  try {
    while (!isTerminating()) {
      UINT32 messageId = m_gate->readUInt32();
      UINT32 messageSize = m_gate->readUInt32();

      m_log->detail(_T("Recieved control message ID %u, size %u"),
                  (unsigned int)messageId, (unsigned int)messageSize);

      bool requiresControlAuth = Configurator::getInstance()->getServerConfig()->isControlAuthEnabled();
      bool repeatAuthEnabled = Configurator::getInstance()->getServerConfig()->getControlAuthAlwaysChecking();

       // Check if message requires TightVNC admin privilegies.
      if (requiresControlAuth) {
        for (size_t i = 0; i < sizeof(WITHOUT_AUTH) / sizeof(UINT32); i++) {
          if (messageId == WITHOUT_AUTH[i]) {
            requiresControlAuth = false;
            break;
          }
        }
      }

      try {
        // Message requires control auth: skip message body and sent reply.
        if (requiresControlAuth) {
          bool authPassed = m_authPassed;
          if (repeatAuthEnabled) {
            authPassed = authPassed && m_authReqMessageId == messageId && m_repeatAuthPassed;
          }
          m_repeatAuthPassed = false;
          if (!authPassed) {
            m_log->detail(_T("Message requires control authentication"));

            m_gate->skipBytes(messageSize);
            m_gate->writeUInt32(ControlProto::REPLY_AUTH_NEEDED);
            m_authReqMessageId = messageId;

            continue;
          }
        }

        switch (messageId) {
        case ControlProto::AUTH_MSG_ID:
          m_log->detail(_T("Control authentication requested"));
          authMsgRcdv();
          break;
        case ControlProto::RELOAD_CONFIG_MSG_ID:
          m_log->detail(_T("Command requested: Reload configuration"));
          reloadConfigMsgRcvd();
          break;
        case ControlProto::DISCONNECT_ALL_CLIENTS_MSG_ID:
          m_log->detail(_T("Command requested: Disconnect all clients command requested"));
          disconnectAllMsgRcvd();
          break;
        case ControlProto::SHUTDOWN_SERVER_MSG_ID:
          m_log->detail(_T("Command requested: Shutdown command requested"));
          shutdownMsgRcvd();
          break;
        case ControlProto::ADD_CLIENT_MSG_ID:
          m_log->detail(_T("Command requested: Attach listening viewer"));
          addClientMsgRcvd();
          break;
        case ControlProto::CONNECT_TO_TCPDISP_MSG_ID:
          m_log->message(_T("Connect to a tcp dispatcher command requested"));
          connectToTcpDispatcher();
          break;
        case ControlProto::GET_SERVER_INFO_MSG_ID:
          m_log->detail(_T("Control client requests server info"));
          getServerInfoMsgRcvd();
          break;
        case ControlProto::GET_CLIENT_LIST_MSG_ID:
          m_log->detail(_T("Control client requests client list"));
          getClientsListMsgRcvd();
          break;
        case ControlProto::SET_CONFIG_MSG_ID:
          m_log->detail(_T("Control client sends new server config"));
          setServerConfigMsgRcvd();
          break;
        case ControlProto::GET_CONFIG_MSG_ID:
          m_log->detail(_T("Control client requests server config"));
          getServerConfigMsgRcvd();
          break;
        case ControlProto::GET_SHOW_TRAY_ICON_FLAG:
          m_log->detail(_T("Control client requests tray icon visibility flag"));
          getShowTrayIconFlagMsgRcvd();
          break;
        case ControlProto::UPDATE_TVNCONTROL_PROCESS_ID_MSG_ID:
          m_log->detail(_T("Control client sends process ID"));
          updateTvnControlProcessIdMsgRcvd();
          break;
        case ControlProto::SHARE_PRIMARY_MSG_ID:
          m_log->message(_T("Share primary message recieved"));
          sharePrimaryIdMsgRcvd();
          break;
        case ControlProto::SHARE_DISPLAY_MSG_ID:
          m_log->message(_T("Share display message recieved"));
          shareDisplayIdMsgRcvd();
          break;
        case ControlProto::SHARE_WINDOW_MSG_ID:
          m_log->message(_T("Share window message recieved"));
          shareWindowIdMsgRcvd();
          break;
        case ControlProto::SHARE_RECT_MSG_ID:
          m_log->message(_T("Share rect message recieved"));
          shareRectIdMsgRcvd();
          break;
        case ControlProto::SHARE_FULL_MSG_ID:
          m_log->message(_T("Share full message recieved"));
          shareFullIdMsgRcvd();
          break;
        case ControlProto::SHARE_APP_MSG_ID:
          m_log->message(_T("Share app message recieved"));
          shareAppIdMsgRcvd();
          break;
        default:
          m_gate->skipBytes(messageSize);
          m_log->warning(_T("Received unsupported message from control client"));
          throw ControlException(_T("Unknown command"));
        } // switch (messageId).
      } catch (ControlException &controlEx) {
        m_log->error(_T("Exception while processing control client's request: \"%s\""),
                   controlEx.getMessage());

        sendError(controlEx.getMessage());
      }
    } // while
  } catch (Exception &ex) {
    m_log->error(_T("Exception in control client thread: \"%s\""), ex.getMessage());
  }
}

void ControlClient::onTerminate()
{
  try { m_transport->close(); } catch (...) { }
}

void ControlClient::sendError(const TCHAR *message)
{
  m_gate->writeUInt32(ControlProto::REPLY_ERROR);
  m_gate->writeUTF8(message);
}

//
// FIXME: Code duplicate (see RfbInitializer class).
//

void ControlClient::authMsgRcdv()
{
  UINT8 challenge[16];
  UINT8 response[16];

  srand((unsigned)time(0));
  for (int i = 0; i < sizeof(challenge); i++) {
    challenge[i] = rand() & 0xff;
  }

  m_gate->writeFully(challenge, sizeof(challenge));
  m_gate->readFully(response, sizeof(response));

  //
  // FIXME: Is it right to check if password is set after client
  // sent password to us.
  //

  ServerConfig *config = Configurator::getInstance()->getServerConfig();
  UINT8 cryptPassword[8];
  config->getControlPassword(cryptPassword);

  bool isAuthSucceed = m_authenticator->authenticate(cryptPassword,
                                                     challenge,
                                                     response);
  if (!isAuthSucceed) {
    sendError(StringTable::getString(IDS_INVALID_CONTROL_PASSWORD));
  } else {
    m_gate->writeUInt32(ControlProto::REPLY_OK);
    m_authPassed = true;
    m_repeatAuthPassed = true;
  }
}

void ControlClient::getClientsListMsgRcvd()
{
  UINT32 clientCount = 0;

  RfbClientInfoList clients;

  m_rfbClientManager->getClientsInfo(&clients);

  m_gate->writeUInt32(ControlProto::REPLY_OK);
  _ASSERT(clients.size() == (unsigned int)clients.size());
  m_gate->writeUInt32((unsigned int)clients.size());

  for (RfbClientInfoList::iterator it = clients.begin(); it != clients.end(); it++) {
    m_gate->writeUInt32((*it).m_id);
    m_gate->writeUTF8((*it).m_peerAddr.getString());
  }
}

void ControlClient::getServerInfoMsgRcvd()
{
  bool acceptFlag = false;
  bool serviceFlag = false;

  StringStorage logPath;
  StringStorage statusText;

  TvnServerInfo info;

  TvnServer::getInstance()->getServerInfo(&info);

  StringStorage status;
  {
    AutoLock al(&m_tcpDispValuesMutex);
    if (m_tcpDispId != 0) {
      status.format(_T("[ID = %u] %s"),
                    m_tcpDispId,
                    info.m_statusText.getString());
    } else {
      status.setString(info.m_statusText.getString());
    }
  }

  m_gate->writeUInt32(ControlProto::REPLY_OK);

  m_gate->writeUInt8(info.m_acceptFlag ? 1 : 0);
  m_gate->writeUInt8(info.m_serviceFlag ? 1 : 0);
  m_gate->writeUTF8(status.getString());
}

void ControlClient::reloadConfigMsgRcvd()
{
  m_gate->writeUInt32(ControlProto::REPLY_OK);

  Configurator::getInstance()->load();
}

void ControlClient::disconnectAllMsgRcvd()
{
  m_gate->writeUInt32(ControlProto::REPLY_OK);

  m_rfbClientManager->disconnectAllClients();
}

void ControlClient::shutdownMsgRcvd()
{
  m_gate->writeUInt32(ControlProto::REPLY_OK);

  TvnServer::getInstance()->generateExternalShutdownSignal();
}

void ControlClient::addClientMsgRcvd()
{
  m_gate->writeUInt32(ControlProto::REPLY_OK);

  //
  // Read parameters.
  //

  StringStorage connectString;

  m_gate->readUTF8(&connectString);

  bool viewOnly = m_gate->readUInt8() == 1;

  //
  // Parse host and port from connection string.
  //
  AnsiStringStorage connectStringAnsi(&connectString);
  HostPath hp(connectStringAnsi.getString(), 5500);

  if (!hp.isValid()) {
    return;
  }

  StringStorage host;
  AnsiStringStorage ansiHost(hp.getVncHost());
  ansiHost.toStringStorage(&host);

  //
  // Make outgoing connection in separate thread.
  //
  OutgoingRfbConnectionThread *newConnectionThread =
                               new OutgoingRfbConnectionThread(host.getString(),
                                                               hp.getVncPort(), viewOnly,
                                                               m_rfbClientManager, m_log);

  newConnectionThread->resume();

  ZombieKiller::getInstance()->addZombie(newConnectionThread);
}

void ControlClient::connectToTcpDispatcher()
{
  m_gate->writeUInt32(ControlProto::REPLY_OK);

  // Read a hostname.
  StringStorage connectString;
  m_gate->readUTF8(&connectString);

  // Read a dispatcher name.
  StringStorage dispatcherName;
  m_gate->readUTF8(&dispatcherName);
  // Read a keyword.
  StringStorage keyword;
  m_gate->readUTF8(&keyword);
  // Read a connection id.
  UINT32 connectionId = m_gate->readUInt32();

  // Parse host and port from connection string.
  AnsiStringStorage connectStringAnsi(&connectString);
  HostPath hp(connectStringAnsi.getString(), 5900);
  if (!hp.isValid()) {
    return;
  }
  StringStorage host;
  AnsiStringStorage ansiHost(hp.getVncHost());
  ansiHost.toStringStorage(&host);

  // Converting got TCHAR strings to AnsiStringStorage format
  AnsiStringStorage ansiDispName(&dispatcherName);
  AnsiStringStorage ansiKeyword(&keyword);

  // Make connection in separate thread.
  ConnectToTcpDispatcherThread *newConnectionThread =
    new ConnectToTcpDispatcherThread(host.getString(),
                                     hp.getVncPort(),
                                     &ansiDispName,
                                     connectionId,
                                     &ansiKeyword,
                                     m_rfbClientManager,
                                     m_log);

  ZombieKiller::getInstance()->addZombie(newConnectionThread);
}

void ControlClient::setServerConfigMsgRcvd()
{
  m_gate->writeUInt32(ControlProto::REPLY_OK);

  Configurator::getInstance()->getServerConfig()->deserialize(m_gate);
  Configurator::getInstance()->save();
  Configurator::getInstance()->load();
}

void ControlClient::getShowTrayIconFlagMsgRcvd()
{
  bool showIcon = Configurator::getInstance()->getServerConfig()->getShowTrayIconFlag();

  m_gate->writeUInt32(ControlProto::REPLY_OK);

  m_gate->writeUInt8(showIcon ? 1 : 0);
}

void ControlClient::updateTvnControlProcessIdMsgRcvd()
{
  m_gate->readUInt32();

  try {
    WTS::duplicatePipeClientToken(m_pipeHandle);
  } catch (Exception &e) {
    m_log->error(_T("Can't update the control client impersonation token: %s"),
               e.getMessage());
  }
  m_gate->writeUInt32(ControlProto::REPLY_OK);
}

void ControlClient::getServerConfigMsgRcvd()
{
  m_gate->writeUInt32(ControlProto::REPLY_OK);

  Configurator::getInstance()->getServerConfig()->serialize(m_gate);
}

void ControlClient::sharePrimaryIdMsgRcvd()
{
  m_gate->writeUInt32(ControlProto::REPLY_OK);
  ViewPortState dynViewPort;
  dynViewPort.setPrimaryDisplay();
  m_rfbClientManager->setDynViewPort(&dynViewPort);
}

void ControlClient::shareDisplayIdMsgRcvd()
{
  unsigned char displayNumber = m_gate->readUInt8();
  m_gate->writeUInt32(ControlProto::REPLY_OK);

  ViewPortState dynViewPort;
  dynViewPort.setDisplayNumber(displayNumber);
  m_rfbClientManager->setDynViewPort(&dynViewPort);
}

void ControlClient::shareWindowIdMsgRcvd()
{
  StringStorage windowName;
  m_gate->readUTF8(&windowName);

  m_gate->writeUInt32(ControlProto::REPLY_OK);

  ViewPortState dynViewPort;
  dynViewPort.setWindowName(&windowName);
  m_rfbClientManager->setDynViewPort(&dynViewPort);
}

void ControlClient::shareRectIdMsgRcvd()
{
  Rect shareRect;
  shareRect.left = m_gate->readInt32();
  shareRect.top = m_gate->readInt32();
  shareRect.right = m_gate->readInt32();
  shareRect.bottom = m_gate->readInt32();
  m_gate->writeUInt32(ControlProto::REPLY_OK);

  ViewPortState dynViewPort;
  dynViewPort.setArbitraryRect(&shareRect);
  m_rfbClientManager->setDynViewPort(&dynViewPort);
}

void ControlClient::shareFullIdMsgRcvd()
{
  m_gate->writeUInt32(ControlProto::REPLY_OK);

  ViewPortState dynViewPort;
  dynViewPort.setFullDesktop();
  m_rfbClientManager->setDynViewPort(&dynViewPort);
}

void ControlClient::shareAppIdMsgRcvd()
{
  unsigned int procId = m_gate->readUInt32();
  m_gate->writeUInt32(ControlProto::REPLY_OK);

  ViewPortState dynViewPort;
  dynViewPort.setProcessId(procId);
  m_rfbClientManager->setDynViewPort(&dynViewPort);
}
