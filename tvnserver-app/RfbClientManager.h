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

#ifndef __RFBCLIENTMANAGER_H__
#define __RFBCLIENTMANAGER_H__

#include "util/ListenerContainer.h"
#include "rfb-sconn/RfbClient.h"
#include "thread/AutoLock.h"
#include "thread/Thread.h"
#include "thread/LocalMutex.h"
#include "win-system/WindowsEvent.h"
#include "desktop/Desktop.h"
#include "desktop/DesktopFactory.h"
#include "log-writer/LogWriter.h"

// Listener interfaces
#include "RfbClientManager.h"
#include "RfbClientManagerEventListener.h"
#include "rfb-sconn/ClientTerminationListener.h"
#include "desktop/ClipboardListener.h"
#include "desktop/AbnormDeskTermListener.h"
#include "desktop/UpdateSendingListener.h"
#include "rfb-sconn/ClientAuthListener.h"
#include "tvncontrol-app/RfbClientInfo.h"
#include "NewConnectionEvents.h"

typedef std::list<RfbClient *> ClientList;
typedef std::list<RfbClient *>::iterator ClientListIter;

struct BanProp
{
  unsigned int count;
  DateTime banFirstTime;
};
typedef std::map<StringStorage, BanProp> BanList;
typedef BanList::iterator BanListIter;

//
// FIXME: RfbClientManager don't care about configuration changes when
// application is running. Is this fixme deprecated?
//
// FIXME: No documentation for problem which this class solves.
class RfbClientManager: public ClientTerminationListener,
                        public ClipboardListener,
                        public UpdateSendingListener,
                        public ClientAuthListener,
                        public AbnormDeskTermListener,
                        public ListenerContainer<RfbClientManagerEventListener *>
{
public:
  // FIXME: parameter is not used.
  RfbClientManager(const TCHAR *serverName,
                   NewConnectionEvents *newConnectionEvents,
                   LogWriter *log,
                   DesktopFactory *desktopFactory);
  virtual ~RfbClientManager();

  // Adds rfb clients info to specified rfb client info list.
  // FIXME: This method needed only for control server.
  void getClientsInfo(RfbClientInfoList *list);

  // Disconnects all connected clients.
  virtual void disconnectAllClients();
  virtual void disconnectNonAuthClients();
  virtual void disconnectAuthClients();

  // Sets a view port value to all client that already run and
  // will be run.
  void setDynViewPort(const ViewPortState *dynViewPort);

  // FIXME: Place comment for this method here.
  void addNewConnection(SocketIPv4 *socket, ViewPortState *constViewPort,
                        bool viewOnly, bool isOutgoing);

protected:
  // Listen functions
  virtual void onClientTerminate();
  virtual Desktop *onClientAuth(RfbClient *client);
  virtual bool onCheckForBan(RfbClient *client);
  // This function only adds the client to the ban list.
  virtual void onAuthFailed(RfbClient *client);
  virtual void onCheckAccessControl(RfbClient *client) throw(AuthException);
  virtual void onClipboardUpdate(const StringStorage *newClipboard);
  virtual void onSendUpdate(const UpdateContainer *updateContainer,
                            const CursorShape *cursorShape);
  virtual bool isReadyToSend();
  // If an error occured RfbClientManager closes all current connections
  // (authorized and not authorized) that bring to closing the belonged desktop
  // object.
  virtual void onAbnormalDesktopTerminate();

  void waitUntilAllClientAreBeenDestroyed();

private:
  void validateClientList();

  // Checks the ip to ban.
  // Returns true if client is banned.
  bool checkForBan(const StringStorage *ip);
  // If the success param is true the belonged ip entry will be removed
  // from the ban list. Else the ip will be added to the ban or will be
  // increased it count.
  void updateIpInBan(const StringStorage *ip, bool success);
  // Removes deprecated bans from the ban list.
  void refreshBan();

  ClientList m_nonAuthClientList;
  ClientList m_clientList;
  LocalMutex m_clientListLocker;
  // m_dynViewPort is a client view port that can be changed during a
  // client work. Now, the dynViewPort has the same value for all clients.
  // By this field initilizes new clients.
  // Acces to the viewport must be covered by the m_clientListLocker mutex.
  ViewPortState m_dynViewPort;

  static const int MAX_BAN_COUNT = 10;
  static const int BAN_TIME = 3000 * MAX_BAN_COUNT; // milliseconds
  BanList m_banList;
  WindowsEvent m_banTimer;
  LocalMutex m_banListMutex;

  WindowsEvent m_listUnderflowingEvent;

  // Creating and destroying this object must be with the locked
  // m_clientListLocker
  Desktop *m_desktop;
  DesktopFactory *m_desktopFactory;

  // Inforamtion
  unsigned int m_nextClientId;

  NewConnectionEvents *m_newConnectionEvents;

  LogWriter *m_log;
};

#endif // __RFBCLIENTMANAGER_H__
