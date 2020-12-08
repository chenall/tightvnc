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

#ifndef _ECHO_EXTENSION_REQUEST_HANDLER_H_
#define _ECHO_EXTENSION_REQUEST_HANDLER_H_

#include "util/inttypes.h"
#include "network/RfbInputGate.h"
#include "network/RfbOutputGate.h"
#include "rfb-sconn/RfbCodeRegistrator.h"
#include "rfb-sconn/RfbDispatcherListener.h"
#include "log-writer/LogWriter.h"

/**
 * Handler of echo extension plugin client to server messages.
 * Processes client requests and sends replies.
 */
class EchoExtensionRequestHandler : public RfbDispatcherListener
{
public:
  /**
   * Creates new echo extension client messages handler.
   * @param registrator rfb registrator which needs to register echo messages
   *   to rfb dispatcher address whem to this object.
   * @param output gate for writting replies for requests.
   * @pararm enabled indicates if echo response should be enabled or disabled
   */
EchoExtensionRequestHandler(RfbCodeRegistrator *registrator,
                             RfbOutputGate *output,
                             LogWriter *log,
                             bool enabled = true);

  /**
   * Deletes echo extension request handler.
   */
  virtual ~EchoExtensionRequestHandler();

  /**
   * Inherited from RfbDispatcherListener.
   * Processes echo extension client messages.
   */
  virtual void onRequest(UINT32 reqCode, RfbInputGate *backGate);
  
  bool isEchoExtensionEnabled();

protected:
  //
  // Input and output gates.
  //

  RfbInputGate *m_input;
  RfbOutputGate *m_output;

  bool m_enabled;
  LogWriter *m_log;
};

#endif //_ECHO_EXTENSION_REQUEST_HANDLER_H_
