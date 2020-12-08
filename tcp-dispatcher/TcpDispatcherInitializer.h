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

// FIXME: This code is almost a duplicate of DispatcherProtocol.
//        We should always use DispatcherProtocol instead of this one.

#ifndef _TCPDISPATCHERINITIALIZER_H_
#define _TCPDISPATCHERINITIALIZER_H_

#include "io-lib/Channel.h"
#include "io-lib/DataOutputStream.h"
#include "io-lib/DataInputStream.h"
#include "util/AnsiStringStorage.h"
#include "log-writer/LogWriter.h"

class InvalidTcpDispProtoType : public Exception
{
public:
  InvalidTcpDispProtoType(const TCHAR *message);
};

class TcpDispatcherInitializer
{
public:
  TcpDispatcherInitializer(Channel *channel, LogWriter *log);
  // sendDispatcherName - a dispatcher name that will be sent
  // to the connected dispatcher. The sendDispatcherName string can be empty.
  // connectionType - false for server, true for viewer.
  // connectionId - a connection id. If the user already has a connection id
  // the value shouldn't be zero and if the user want get a connection id the
  // value should be zero.
  // keyword - a string that certify the user. If the certification does not
  // require the string should be empty.
  TcpDispatcherInitializer(Channel *channel,
                           const AnsiStringStorage *sendDispatcherName,
                           bool connectionType,
                           UINT32 connectionId,
                           const AnsiStringStorage *keyword,
                           LogWriter *log);
  ~TcpDispatcherInitializer();

  // This function must be called if a TcpDispatcherInitializer object has been
  // created by calling the TcpDispatcherInitializer(Channel *channel)
  // constructor. In this case this function must be called before calling
  // the continueTcpDispatchProtocol() function. The readProtocolType()
  // function can be called before or after the init() function calling.
  void init(const AnsiStringStorage *sendDispatcherName,
            bool connectionType,
            UINT32 connectionId,
            const AnsiStringStorage *keyword);

  // To process the tcp dispatcher protocol call this three function
  // in the order as it stay here.

  // Read first 12 bytes and fills the m_protocolType string by the
  // read letters in the TCHAR format.
  // If the bytes isn't the TcpDispatcher values the function throwing
  // the TcpDispatcherException.
  void readProtocolType();
  // After calling this function all values will be known such as
  // connection id, dispatcher name.
  void continueTcpDispatchProtocol();
  void waitNextProtocolContinue();

  void getProtocolTypeString(StringStorage *protoStr);
  UINT32 getConnectionId();
  void getRecivedDispatcherName(AnsiStringStorage *dispName);

private:
  void negotiateProtocolVersion();
  void sendConnectionType();
  void negotiateConnectionId();
  void sendKeyword();
  void exchangeDispatcherNames();

  DataOutputStream m_output;
  DataInputStream m_input;

  StringStorage m_protocolType;
  AnsiStringStorage m_sendDispatcherName;
  AnsiStringStorage m_gotDispatcherName;
  bool m_connectionType;
  UINT32 m_connectionId;
  AnsiStringStorage m_keyword;

  LogWriter *m_log;
};

#endif // _TCPDISPATCHERINITIALIZER_H_
