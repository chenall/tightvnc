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

#ifndef __RFBCLIENT_H__
#define __RFBCLIENT_H__

#include <list>
#include "network/socket/SocketIPv4.h"
#include "win-system/WindowsEvent.h"
#include "thread/Thread.h"
#include "network/RfbOutputGate.h"
#include "desktop/Desktop.h"
#include "fb-update-sender/UpdateSender.h"
#include "log-writer/LogWriter.h"

#include "RfbDispatcher.h"
#include "ClipboardExchange.h"
#include "ClientInputHandler.h"
#include "ClientTerminationListener.h"
#include "ClientInputEventListener.h"
#include "tvnserver-app/NewConnectionEvents.h"
#include "util/DemandTimer.h"

class ClientAuthListener;

// FIXME: Document it.
enum ClientState
{
  IN_NONAUTH,
  IN_AUTH,
  IN_NORMAL_PHASE,
  IN_PENDING_TO_REMOVE,
  IN_READY_TO_REMOVE
};

// FIXME: Document it, i understand nothing from such kind of description.
class RfbClient: public Thread, ClientInputEventListener, private SenderControlInformationInterface
{
public:
  RfbClient(NewConnectionEvents *newConnectionEvents, SocketIPv4 *socket,
            ClientTerminationListener *extTermListener,
            ClientAuthListener *extAuthListener, bool viewOnly,
            bool isOutgoing, unsigned int id,
            const ViewPortState *constViewPort,
            const ViewPortState *dynViewPort,
            int idleTimeout,
            LogWriter *log);
  virtual ~RfbClient();

  void disconnect();

  ClientState getClientState();

  unsigned int getId() const;
  void getPeerHost(StringStorage *address);
  void getLocalIpAddress(StringStorage *address);
  void getSocketAddr(SocketAddressIPv4 *addr) const;

  // Return true if connection has been initialised from the server to a client
  // else return false.
  bool isOutgoing() const;

  bool getSharedFlag() const { return m_shared; }
  bool getViewOnlyAuth() const { return m_viewOnlyAuth; }

  void setViewOnlyFlag(bool value);

  // Changes current dynViewPort value by new.
  void changeDynViewPort(const ViewPortState *dynViewPort);

  bool clientIsReady() const { return m_updateSender->clientIsReady(); }
  void sendUpdate(const UpdateContainer *updateContainer,
                  const CursorShape *cursorShape);
  void sendClipboard(const StringStorage *newClipboard);

protected:
  virtual void execute();
  virtual void onTerminate();

private:
  // Calling this function makes the client manager enter (and leave) the
  // mutex associated with the client list, so it will have to wait until
  // other threads stop working with our object (such operations should be
  // protected with the same mutex as well). If we call this function to
  // change the state to IN_PENDING_TO_REMOVE or IN_READY_TO_REMOVE, we can
  // guarantee that our object will not be used by the client manager after
  // this call.
  void notifyAbStateChanging(ClientState state);

  // This class is layer between WinDesktop and ClientInputHandler.
  virtual void onKeyboardEvent(UINT32 keySym, bool down);
  virtual void onMouseEvent(UINT16 x, UINT16 y, UINT8 buttonMask);

  void setClientState(ClientState newState);

  Rect getViewPortRect(const Dimension *fbDimension);
  virtual void onGetViewPort(Rect *viewRect, bool *shareApp, Region *shareAppRegion);
  void getViewPortInfo(const Dimension *fbDimension, Rect *resultRect,
                       bool *shareApp, Region *shareAppRegion);

  ClientState m_clientState;
  bool m_isMarkedOk;
  LocalMutex m_clientStateMut;
  ClientTerminationListener *m_extTermListener;

  SocketIPv4 *m_socket;

  ClientAuthListener *m_extAuthListener;

  ViewPort m_constViewPort;
  ViewPort m_dynamicViewPort;
  LocalMutex m_viewPortMutex;

  UpdateSender *m_updateSender;
  ClipboardExchange *m_clipboardExchange;
  ClientInputHandler *m_clientInputHandler;
  Desktop *m_desktop;

  bool m_viewOnly;
  bool m_isOutgoing;
  bool m_viewOnlyAuth;
  bool m_shared;

  LogWriter *m_log;

  // Information
  unsigned int m_id;

  NewConnectionEvents *m_newConnectionEvents;
  // This timer sets by IdleTimeout value from server config 
  // and resets on mouse or keyboard event
  DemandTimer m_idleTimer;
  int m_idleTimeout;
};

#endif // __RFBCLIENT_H__
