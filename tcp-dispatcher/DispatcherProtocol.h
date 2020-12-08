// Copyright (C) 2011,2012,2013,2014 GlavSoft LLC.
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

// FIXME: Correct the documentation!

#ifndef __DISPATCHER_PROTOCOL_H__
#define __DISPATCHER_PROTOCOL_H__

#include "network/RfbInputGate.h"
#include "network/RfbOutputGate.h"
#include "util/AnsiStringStorage.h"
#include "log-writer/LogWriter.h"

class BadDispatcherProtocolException : public Exception
{
public:
  BadDispatcherProtocolException(const TCHAR *message);
};

class DispatcherProtocol
{
public:
  DispatcherProtocol(RfbInputGate *inputGate, RfbOutputGate *outputGate, LogWriter *log);
  // sendDispatcherName - a dispatcher name that will be sent
  // to the connected dispatcher. The sendDispatcherName string can be empty.
  // connectionType - false for server, true for viewer.
  // connectionId - a connection id. If the user already has a connection id
  // the value shouldn't be zero and if the user want get a connection id the
  // value should be zero.
  // keyword - a string that certify the user. If the certification does not
  // require the string should be empty.
  DispatcherProtocol(RfbInputGate *inputGate, RfbOutputGate *outputGate,
                     const char *sendDispatcherName,
                     bool connectionType,
                     UINT32 connectionId,
                     const char *keyword,
                     LogWriter *log);
  ~DispatcherProtocol();

  // This function must be called if a DispatcherProtocol object has been
  // created by calling the DispatcherProtocol(RfbInputGate *, RfbOutputGate *)
  // constructor. In this case this function must be called before calling
  // the continueTcpDispatchProtocol() function. The readProtocolType()
  // function can be called before or after the init() function calling.
  void init(const char *sendDispatcherName,
            bool connectionType,
            UINT32 connectionId,
            const char *keyword);

  // Check if a given protocol identification string matches known signature.
  static bool checkProtocolSignature(const char str[13]);

  // To process the Dispatcher protocol call the following three functions in this order.

  // Read the protocol identification signature (first 12 bytes). If it does not match known
  // signature of the Dispatcher protocol, the function throws BadDispatcherProtocolException.
  void readProtocolType();

  // After calling this function all values will be known such as
  // connection id, dispatcher name.
  void continueTcpDispatchProtocol();
  void waitNextProtocolContinue();

  UINT32 getConnectionId();
  void getRecivedDispatcherName(AnsiStringStorage *dispName);

private:
  void negotiateProtocolVersion();
  void sendConnectionType();
  void negotiateConnectionId();
  void sendKeyword();
  void exchangeDispatcherNames();

  RfbInputGate *m_input;
  RfbOutputGate *m_output;

  AnsiStringStorage m_sendDispatcherName;
  AnsiStringStorage m_gotDispatcherName;
  bool m_connectionType;
  UINT32 m_connectionId;
  AnsiStringStorage m_keyword;

  LogWriter *m_log;
};

#endif // __DISPATCHER_PROTOCOL_H__
