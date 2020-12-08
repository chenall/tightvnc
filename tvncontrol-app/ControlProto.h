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

#ifndef _CONTROL_PROTO_H_
#define _CONTROL_PROTO_H_

#include "util/CommonHeader.h"

/**
 * Control protocol constants.
 *
 * @section Client requests.
 * Client to server control messages are request-reply oriented and have format
 * descripbed above:
 *
 * UINT32 command code.
 * UINT32 message body size (in bytes).
 * UINT32[] message body (byte array with length that equals to message body size).
 *
 * @section Server replies.
 * Server to client messages (eq replies to client requests) have following format:
 * UINT32 reply code (REPLY_OK - no error, REPLY_ERROR - error during request processing,
 * REPLY_AUTH_NEEDED - authorization needed for this request).
 * UINT8[] body (reply body with unknown length (i mean body is individual for every reply,
 * some replies can have no body).
 */
class ControlProto
{
public:
  /**
   * Reply code OK (request processed, reply body contains result).
   */
  static const UINT32 REPLY_OK = 0x0;

  /**
   * Reply code ERROR (failed to process request, reply body contains error description
   * in UTF8).
   */
  static const UINT32 REPLY_ERROR = 0x1;

  /**
   * Reply code ERROR AUTH NEEDED (your requested wasn't processed and you
   * need to authorize before you can execute your request).
   */
  static const UINT32 REPLY_AUTH_NEEDED = 0x2;

  /**
   * Get list of clients of rfb server.
   *
   * Request body: [empty].
   * Reply body:
   *   UINT32 clientsCount.
   *   struct {
   *     UINT32 clientId.
   *     StringUTF8 peerAddr.
   *   } clientsInfo[clientsCount].
   */
  static const UINT32 GET_CLIENT_LIST_MSG_ID = 0x4;

  /**
   * Reload rfb server configuration.
   *
   * Request body: [empty].
   * Reply body: [empty].
   *
   * @deprecated.
   */
  static const UINT32 RELOAD_CONFIG_MSG_ID = 0x5;

  /**
   * Disconnect all clients from rfb server.
   * Request body: [empty].
   * Reply body: [empty].
   */
  static const UINT32 DISCONNECT_ALL_CLIENTS_MSG_ID = 0x6;

  /**
   * Shutdown TightVNC server.
   * Request body: [empty].
   * Reply body: [empty].
   */
  static const UINT32 SHUTDOWN_SERVER_MSG_ID = 0x7;

  /**
   * Make outgoing (reverse) rfb connection from server.
   * Request body:
   *   StringUTF8 connectString.
   *   UINT8 viewOnlyFlag.
   * Reply body: [empty].
   */
  static const UINT32 ADD_CLIENT_MSG_ID = 0x8;

  /**
   * Make connection to a TcpDispatcher from server.
   * Request body:
   *   StringUTF8 connectString.
   * Reply body: [empty].
   */
  static const UINT32 CONNECT_TO_TCPDISP_MSG_ID = 0x9;

  /**
   * Send new configuration to TightVNC server.
   * Request body:
   *   serialized ServerConfig.
   * Reply body: [empty].
   */
  static const UINT32 SET_CONFIG_MSG_ID = 0x10;

  /**
   * Get server status info.
   *
   * Request body: [empty].
   * Reply body:
   *   UINT8 connectionAcceptFlag.
   *   UINT8 isServiceFlag.
   *   StringUTF8 logFilePath.
   *   StringUTF8 serverStatusText.
   */
  static const UINT32 GET_SERVER_INFO_MSG_ID = 0x11;

  /**
   * Get current configuration of TightVNC server.
   * Request body: [empty].
   * Reply body:
   *   serialized ServerConfig.
   */
  static const UINT32 GET_CONFIG_MSG_ID = 0x12;

  /**
   * Client to server messages.
   */

  /**
   * Special message to make to authorize client using VNC-style auth.
   * After server recieves this message VNC-style auth must be done.
   */
  static const UINT32 AUTH_MSG_ID = 0x13;

  /**
   * Gets flag for autowork mode of TvnControl.
   *
   * @remark
   * If this flag is true when TvnControl must be run with TightVNC app or service,
   * if false, then TvnControl app must be shutdowned forced and silent after it knows
   * that "Run TvnControl flag" is false.
   *
   * Request body: [empty].
   * Reply body:
   *   UINT8 runTvnControlFlag.
   */
  static const UINT32 GET_SHOW_TRAY_ICON_FLAG = 0x14;

  /**
   * Updates tvncontrol process id on server side.
   * Request body:
   *  UINT32 processId.
   * Reply body: [empty].
   */
  static const UINT32 UPDATE_TVNCONTROL_PROCESS_ID_MSG_ID = 0x15;

  // Send to server a command that to share only a primary desktop.
  static const UINT32 SHARE_PRIMARY_MSG_ID = 0x20;

  // Send to server a command to share only the display.
  static const UINT32 SHARE_DISPLAY_MSG_ID = 0x21;

  // Send to server a command to share rect constrained by a window.
  static const UINT32 SHARE_WINDOW_MSG_ID = 0x22;

  // Send to server a command to share only the rect.
  static const UINT32 SHARE_RECT_MSG_ID = 0x23;

  // Send to server a command to share only the rect.
  static const UINT32 SHARE_FULL_MSG_ID = 0x24;

  // Send to server a command to share only the rect.
  static const UINT32 SHARE_APP_MSG_ID = 0x25;
};

#endif
