// Copyright (C) 2008,2009,2010,2011,2012 GlavSoft LLC.
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

#ifndef _SERVER_CONFIG_H_
#define _SERVER_CONFIG_H_

#include "util/StringVector.h"
#include "util/Exception.h"
#include "PortMappingContainer.h"
#include "IpAccessControl.h"
#include "thread/AutoLock.h"
#include "thread/LocalMutex.h"
#include "IpAccessRule.h"
#include "io-lib/DataInputStream.h"
#include "io-lib/DataOutputStream.h"
#include "io-lib/IOException.h"
#include "region/RectSerializer.h"

#include <shlobj.h>

class ServerConfig : public Lockable
{
public:
  static const unsigned int MINIMAL_POLLING_INTERVAL = 30;
  static const unsigned int MINIMAL_LOCAL_INPUT_PRIORITY_TIMEOUT = 1;
  static const unsigned int MINIMAL_QUERY_TIMEOUT = 1;

  // FIXME: duplicatad at VncPassCrypt
  static const int VNC_PASSWORD_SIZE = 8;

  //
  // Enum defines server action when last client disconnects
  // from the TightVNC server.
  //

  enum DisconnectAction {
    DA_DO_NOTHING = 0,
    DA_LOCK_WORKSTATION = 1,
    DA_LOGOUT_WORKSTATION = 2
  };

public:
  ServerConfig();
  virtual ~ServerConfig();

  /**
   * Serializes server config to output stream as byte stream.
   * Thread-safe method.
   * @throws Exception on io error.
   * @fixme stub.
   */
  void serialize(DataOutputStream *output) throw(Exception);

  /**
   * Deserializes server config from input stream.
   * Thread-safe method.
   * @throws Exception on io error.
   * @fixme stub.
   */
  void deserialize(DataInputStream *input) throw(Exception);

  //
  // Inherited from Lockable abstract class.
  //

  virtual void lock() {
    m_objectCS.lock();
  }

  virtual void unlock() {
    m_objectCS.unlock();
  }

  //
  // Display (or port numbers) group.
  //

  bool isControlAuthEnabled();
  void useControlAuth(bool useAuth);

  bool getControlAuthAlwaysChecking();
  void setControlAuthAlwaysChecking(bool value);

  void setRfbPort(int port);
  int getRfbPort();

  void setHttpPort(int port);
  int getHttpPort();

  //
  // Other server options access methods
  //

  void enableFileTransfers(bool enabled);
  bool isFileTransfersEnabled();

  void enableRemovingDesktopWallpaper(bool enabled);
  bool isRemovingDesktopWallpaperEnabled();

  void setDisconnectAction(DisconnectAction action);
  DisconnectAction getDisconnectAction();

  bool getD3DIsAllowed();
  void setD3DAllowing(bool value);

  bool getMirrorIsAllowed();
  void setMirrorAllowing(bool value);

  //
  // Incoming connections options group
  //

  bool isAcceptingRfbConnections();
  void acceptRfbConnections(bool accept);

  void getPrimaryPassword(unsigned char *password);
  void setPrimaryPassword(const unsigned char *value);

  void getReadOnlyPassword(unsigned char *password);
  void setReadOnlyPassword(const unsigned char *value);

  void getControlPassword(unsigned char *password);
  void setControlPassword(const unsigned char *password);

  bool hasPrimaryPassword();
  bool hasReadOnlyPassword();
  bool hasControlPassword();

  void deletePrimaryPassword();
  void deleteReadOnlyPassword();
  void deleteControlPassword();

  //
  // Configurator from Administration tab
  //

  bool isUsingAuthentication();

  void useAuthentication(bool enabled);

  bool isOnlyLoopbackConnectionsAllowed();

  void acceptOnlyLoopbackConnections(bool enabled);

  bool isAcceptingHttpConnections();

  void acceptHttpConnections(bool accept);

  bool isAppletParamInUrlEnabled();

  void enableAppletParamInUrl(bool enabled);

  int getLogLevel();

  void setLogLevel(int logLevel);

  //
  // Sharing configuration
  //

  bool isAlwaysShared();

  bool isNeverShared();

  bool isDisconnectingExistingClients();

  void setAlwaysShared(bool enabled);

  void setNeverShared(bool enabled);

  void disconnectExistingClients(bool disconnectExisting);

  void setPollingInterval(unsigned int interval);

  unsigned int getPollingInterval();

  //
  // Input handling config
  //

  void blockRemoteInput(bool blockEnabled);

  bool isBlockingRemoteInput();

  void setLocalInputPriority(bool localPriority);

  bool isLocalInputPriorityEnabled();

  unsigned int getLocalInputPriorityTimeout();

  void setLocalInputPriorityTimeout(unsigned int value);

  void blockLocalInput(bool enabled);

  bool isBlockingLocalInput();

  //
  // Query config
  //

  unsigned int getQueryTimeout();

  void setQueryTimeout(unsigned int timeout);

  bool isDefaultActionAccept();
  void setDefaultActionToAccept(bool accept);

  //
  // Port mapping config
  //

  // Remark: not-thread safe method, use lock / unlock methods of this class
  // to lock and unlock server configuration.
  PortMappingContainer *getPortMappingContainer();

  //
  // Ip access control config
  //

  // Remark: not-thread safe method, use lock / unlock methods of this class
  // to lock and unlock server configuration.
  IpAccessControl *getAccessControl();

  IpAccessRule::ActionType getActionByAddress(unsigned long ip);

  void allowLoopbackConnections(bool allow);

  bool isLoopbackConnectionsAllowed();

  //
  // Video regions
  //

  // FIXME: Deprecated?
  // Remark: not-thread safe method, use lock / unlock methods of this class
  // to lock and unlock server configuration.
  StringVector *getVideoClassNames();
  
  std::vector<Rect> *getVideoRects();

  //
  // Other
  //

  unsigned int getVideoRecognitionInterval();
  void setVideoRecognitionInterval(unsigned int interval);

  int  getIdleTimeout();
  void setIdleTimeout(int timeout);

  void saveLogToAllUsersPath(bool enabled);
  bool isSaveLogToAllUsersPathFlagEnabled();

  void setGrabTransparentWindowsFlag(bool grab);
  bool getGrabTransparentWindowsFlag();

  bool getShowTrayIconFlag();
  void setShowTrayIconFlag(bool val);

  void getLogFileDir(StringStorage *logFileDir);
  void setLogFileDir(const TCHAR *logFileDir);

protected:

  //
  // Server port numbers
  //

  int m_rfbPort;
  int m_httpPort;

  //
  // Other server options members group
  //

  bool m_enableFileTransfers;
  bool m_removeWallpaper;
  bool m_D3DAllowed;
  bool m_mirrorDriverAllowed;
  //
  // Server action when last client disconnects from server
  //

  DisconnectAction m_disconnectAction;

  //
  // Incoming connections options group
  //

  bool m_acceptRfbConnections;
  bool m_acceptHttpConnections;

  unsigned char m_primaryPassword[VNC_PASSWORD_SIZE];
  unsigned char m_readonlyPassword[VNC_PASSWORD_SIZE];
  unsigned char m_controlPassword[VNC_PASSWORD_SIZE];

  //
  // Configurator from Administration tab
  //

  bool m_useAuthentication;
  bool m_onlyLoopbackConnections;
  bool m_enableAppletParamInUrl;
  int m_logLevel;
  bool m_useControlAuth;
  bool m_controlAuthAlwaysChecking;

  //
  // Sharing configuration
  //

  bool m_alwaysShared;
  bool m_neverShared;
  bool m_disconnectClients;

  //
  // Polling configuration
  //

  unsigned int m_pollingInterval;

  //
  // When flag is set server always blocks remote input.
  //

  bool m_blockRemoteInput;
  //
  // When flag is set server always blocks local input.
  //

  bool m_blockLocalInput;

  //
  // When flag is set server blocks remote input
  // on local input activity.
  //

  bool m_localInputPriority;

  //
  // Local input invactivity timeout during that
  // we still blocking remote input(when m_localInputPriority
  // is enabled).
  //

  unsigned int m_localInputPriorityTimeout;

  bool m_defaultActionAccept;

  //
  // Timeout for Query IpAccessControl record
  //

  unsigned int m_queryTimeout;

  //
  // Port mapping config
  //

  PortMappingContainer m_portMappings;

  //
  // Ip access control config
  //

  IpAccessControl m_accessControlContainer;
  bool m_allowLoopbackConnections;

  //
  // Video regions
  //
  
  // Defined by window class name
  StringVector m_videoClassNames;
  // Defined by rectangle coords in "dXxdY+X0+Y0" format, as in -sharerect command line option
  std::vector<Rect> m_videoRects;
  
  //
  // Other
  //

  unsigned int m_videoRecognitionInterval;
  bool m_grabTransparentWindows;

  // Socket timeout to disconnect inactive clients, in seconds
  int m_idleTimeout;

  // Flag that determiates where log file directory will be.
  bool m_saveLogToAllUsersPath;
  // Run control interface with TightVNC server or not.
  bool m_showTrayIcon;

  StringStorage m_logFilePath;
private:

  //
  // Helper methods
  //

  bool m_hasPrimaryPassword;
  bool m_hasReadOnlyPassword;
  bool m_hasControlPassword;

  //
  // Critical section
  //

  LocalMutex m_objectCS;
};

#endif
