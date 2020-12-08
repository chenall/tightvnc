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

#ifndef _CONTROL_PROXY_H_
#define _CONTROL_PROXY_H_

#include "tvncontrol-app/ControlGate.h"
#include "tvncontrol-app/RfbClientInfo.h"
#include "tvncontrol-app/TvnServerInfo.h"

#include "server-config-lib/ServerConfig.h"

#include "ControlMessage.h"
#include "RemoteException.h"

#include <list>

using namespace std;

/**
 * Proxy to some of TightVNC server methods, supported by control protocol.
 * Used to execute remote commands on on TightVNC server.
 * remote errors.
 */
class ControlProxy
{
public:
  /**
   * Creates proxy.
   * @param gate transport to send and recieve messages.
   */
  ControlProxy(ControlGate *gate);
  /**
   * Class destructor.
   */
  virtual ~ControlProxy();

  // If server requests the authentication then firstly will be used
  // *passwordFile parameter then getPassFromConfigEnabled. Call this function
  // to determine this parameters. If this function has not been called then
  // on server auth request will be used a dialog box.
  void setPasswordProperties(const TCHAR *passwordFile,
                             bool getPassFromConfigEnabled,
                             bool forService);

  /**
   * Returns rfb server info status.
   * @return rfb server info status.
   * @throws RemoteException on error on server.
   * @throws IOException on io error.
   */
  TvnServerInfo getServerInfo() throw(IOException, RemoteException);

  /**
   * Gets rfb client list.
   * @param clients [out] output parameters to retrieve info of clients.
   * @throws RemoteException on error on server.
   * @throws IOException on io error.
   */
  void getClientsList(list<RfbClientInfo *> *clients) throw(IOException, RemoteException);

  /**
   * Reloads rfb server configuration.
   * @throws RemoteException on error on server.
   * @throws IOException on io error.
   * @deprecated.
   */
  void reloadServerConfig() throw(IOException, RemoteException);
  
  /**
   * Disconnects all existing rfb clients from server.
   * @throws RemoteException on error on server.
   * @throws IOException on io error.
   */
  void disconnectAllClients() throw(IOException, RemoteException);

  /**
   * Shutdowns TightVNC server.
   * @throws RemoteException on error on server.
   * @throws IOException on io error.
   */
  void shutdownTightVnc() throw(IOException, RemoteException);

  /**
   * Initialized outgoing rfb connection.
   * @param connectString connect string in host[:(port|diplay)] format.
   * @param viewOnly if rfb connection must be in view only mode.
   * @throws RemoteException on error on server.
   * @throws IOException on io error.
   */
  void makeOutgoingConnection(const TCHAR *connectString, bool viewOnly) throw(IOException, RemoteException);

  /**
   * Initialized connection to a TcpDispatcher.
   * @param connectString connect string in host[:(port|diplay)] format.
   */
  void makeTcpDispatcherConnection(const TCHAR *connectString,
                                   const TCHAR *dispatcherName,
                                   const TCHAR *keyword,
                                   UINT32 connectionId);

  // Share only primary display for all clients.
  void sharePrimary();

  // Share only the display for all clients.
  void shareDisplay(unsigned char displayNumber);

  // Share a rect that constrained by a window form.
  // shareWindowName - is a part of the window header name.
  void shareWindow(const StringStorage *shareWindowName);

  // Share only a rect.
  void shareRect(const Rect *shareRect);

  // Share full desktop for all clients.
  void shareFull();

  // Share only application region for all clients. Other regions must be painted to black.
  void shareApp(unsigned int procId);

  /**
   * Sends new configuration to server.
   * @param config new server configuration.
   * @throws RemoteException on error on server.
   * @throws IOException on io error.
   */
  void setServerConfig(ServerConfig *config) throw(IOException, RemoteException);

  /**
   * Gets current configuration from server.
   * @param config [out] output parameter where configuration will be stored.
   * @throws RemoteException on error on server.
   * @throws IOException on io error.
   */
  void getServerConfig(ServerConfig *config) throw(IOException, RemoteException);

  /**
   * Checks if TvnControl must show icon in tray.
   * @throws IOException on io error, RemoteException on error on server side.
   */
  bool getShowTrayIconFlag() throw(IOException, RemoteException);

  /**
   * Updates TvnControl application process id on server side.
   * @param processId tvncontrol application process id.
   * @throws RemoteException on error on server.
   * @throws IOException on io error.
   */
  void updateTvnControlProcessId(DWORD processId) throw(IOException, RemoteException);

protected:
  /**
   * Returns control message to write.
   * @param messageId control message id.
   */
  ControlMessage *createMessage(DWORD messageId);

protected:
  /**
   * Transport for sending and recieving control proto messages.
   */
  ControlGate *m_gate;
  /**
   * Current control message.
   */
  ControlMessage *m_message;
private:
  /**
   * Deletes control message created by createMessage() method
   * if it's exists.
   */
  void releaseMessage();

  StringStorage m_passwordFile;
  bool m_getPassFromConfigEnabled;
  bool m_forService;
};

#endif
