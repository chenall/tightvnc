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

#ifndef _CONTROL_MESSAGE_H_
#define _CONTROL_MESSAGE_H_

#include "io-lib/DataOutputStream.h"
#include "io-lib/ByteArrayOutputStream.h"

#include "util/CommonHeader.h"

#include "RemoteException.h"
#include "ControlGate.h"

/**
 * Class for sending single control request to control server.
 *
 * Solves problem of calculating control message body size (@see ControlProto for details).
 * and client auth dialog (if server requires auth for request, this class will show auth dialog).
 *
 * You can use this class only for sending ONE control message.
 *
 * @usage
 *   1) Create instance of ControlMessage.
 *   2) Write message body (you don't need to write message id).
 *   3) Call send().
 *   4) Destroy instance created in step 1.
 *   5) Read body of reply using control gate (not using this class).
 *
 * @remark Class writes data into memory though DataOutputStream interface so, methods
 * inherited from DataOutputStream doesn't raise any exception.
 *
 * @remark ControlMessage class does not lock control gate.
 */
class ControlMessage : public DataOutputStream
{
public:
  /**
   * Creates new control message.
   * @param messageId control message id described in ControlProto.
   * @param gate control gate.
   * @param *password. If this param != 0 then it will be uses on auth needed
   * instead of the dialog box.
   * @param getPassFromConfigEnabled. If this param is true then it will be
   * uses on auth needed instead of the dialog box. This parameter ignores if
   * *password parameter is presented.
   */
  ControlMessage(UINT32 messageId, ControlGate *gate,
                 const TCHAR *passwordFile = 0,
                 bool getPassFromConfigEnabled = false,
                 bool forService = false);
  /**
   * Destroys control message without sending it.
   */
  virtual ~ControlMessage();

  /**
   * Sends control message and checks server answer (exceptions).
   * @throws IOException on io error, RemoteException on auth or server error.
   */
  virtual void send() throw(IOException, RemoteException);

private:
  /**
   * Writes control message to control gate.
   * @throws IOException on io error.
   */
  void sendData() throw(IOException);

  /**
   * Checks control message ret code.
   * @throws IOException on io error, RemoteException if there error during executing remote command (method).
   */
  void checkRetCode() throw(IOException, RemoteException);

  // Perform auth with a specified password file.
  void authFromFile();

  // Perform auth with a specified password file.
  void authFromRegistry();

private:
  /**
   * Control gate.
   */
  ControlGate *m_gate;
  /**
   * Tunnel.
   */
  ByteArrayOutputStream *m_tunnel;
  /**
   * Control message id.
   */
  UINT32 m_messageId;

  StringStorage m_passwordFile;
  bool m_getPassFromConfigEnabled;
  bool m_forService;
};

#endif
