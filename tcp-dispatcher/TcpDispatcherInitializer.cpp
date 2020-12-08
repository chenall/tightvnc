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

#include "TcpDispatcherInitializer.h"

InvalidTcpDispProtoType::InvalidTcpDispProtoType(const TCHAR *message)
: Exception(message)
{
}

TcpDispatcherInitializer::TcpDispatcherInitializer(Channel *channel, LogWriter *log)
: m_output(channel),
  m_input(channel),
  m_log(log)
{
}

TcpDispatcherInitializer::TcpDispatcherInitializer(Channel *channel,
                             const AnsiStringStorage *sendDispatcherName,
                             bool connectionType,
                             UINT32 connectionId,
                             const AnsiStringStorage *keyword,
                             LogWriter *log)
: m_output(channel),
  m_input(channel),
  m_log(log),
  m_sendDispatcherName(*sendDispatcherName),
  m_connectionType(connectionType),
  m_connectionId(connectionId),
  m_keyword(*keyword)
{
}

TcpDispatcherInitializer::~TcpDispatcherInitializer()
{
}

void TcpDispatcherInitializer::init(const AnsiStringStorage *sendDispatcherName,
                                    bool connectionType,
                                    UINT32 connectionId,
                                    const AnsiStringStorage *keyword)
{
  m_sendDispatcherName.setString(sendDispatcherName->getString());
  m_connectionType = connectionType;
  m_connectionId = connectionId;
  m_keyword.setString(keyword->getString());
}

void TcpDispatcherInitializer::readProtocolType()
{
  size_t strLength = 12;
  std::vector<char> charBuff(strLength + 1);
  m_input.readFully(&charBuff.front(), strLength); // Read "TCPDISPATCH\n"
  charBuff[strLength] = '\0';
  AnsiStringStorage ansiText(&charBuff.front());
  ansiText.toStringStorage(&m_protocolType);
  if (!m_protocolType.isEqualTo(_T("TCPDISPATCH\n"))) {
    throw InvalidTcpDispProtoType(_T("It is not a tcp dispatcher protocol"));
  }
}

void TcpDispatcherInitializer::continueTcpDispatchProtocol()
{
  negotiateProtocolVersion();
  sendConnectionType();
  negotiateConnectionId();
  sendKeyword();
  exchangeDispatcherNames();
}

void TcpDispatcherInitializer::negotiateProtocolVersion()
{
  // Get supported versions
  UINT8 numSupportedVersion = m_input.readUInt8();
  if (numSupportedVersion == 0) {
    StringStorage errMess;
    errMess.format(_T("Illegal number of supported version: %d"),
                   (int)numSupportedVersion);
    throw Exception(errMess.getString());
  }
  std::vector<UINT8> supportedVersion(numSupportedVersion);
  m_input.readFully(&supportedVersion.front(), supportedVersion.size());

  // Reply a number version that will be used.
  UINT8 effectiveVersion = 0;
  for (size_t i = 0; i < supportedVersion.size(); i++) {
    if (supportedVersion[i] == 3) {
      effectiveVersion = 3;
      break;
    }
  }
  if (effectiveVersion == 0) {
    throw Exception(_T("Can't find effective version"));
  }
  m_output.writeUInt8(effectiveVersion);
}

void TcpDispatcherInitializer::sendConnectionType()
{
  // Send connection type (0 = RfbServer)
  m_output.writeUInt8((UINT8)m_connectionType);
}

void TcpDispatcherInitializer::negotiateConnectionId()
{
  m_output.writeUInt32(m_connectionId);
  m_log->info(_T("sent connection id = %u"), m_connectionId);
  if (m_connectionId == 0) {
    // Get connection Id
    m_connectionId = m_input.readUInt32();
    m_log->info(_T("Got connection id = %u"), m_connectionId);
  }
}

void TcpDispatcherInitializer::sendKeyword()
{
  size_t length = m_keyword.getLength();
  if (length > 255) {
    length = 255;
  }
  m_output.writeUInt8((UINT8)length);
  m_output.writeFully(m_keyword.getString(), length);
}

void TcpDispatcherInitializer::exchangeDispatcherNames()
{
  // Send a predefined dispatcher name
  size_t length = m_sendDispatcherName.getLength();
  if (length > 255) {
    length = 255;
  }
  m_output.writeUInt8((UINT8)length);
  m_output.writeFully(m_sendDispatcherName.getString(), length);

  // Get a dispatcher name from the dispatcher.
  length = m_input.readUInt8();
  std::vector<char> gotName(length + 1);
  m_input.readFully(&gotName.front(), length);
  gotName[length] = 0;
  m_gotDispatcherName.setString(&gotName.front());
}

void TcpDispatcherInitializer::waitNextProtocolContinue()
{
  // Wait until get an allowing byte
  while(m_input.readUInt8() == 0) {
    m_output.writeUInt8(0);
  }
}

void TcpDispatcherInitializer::getProtocolTypeString(StringStorage *protoStr)
{
  *protoStr = m_protocolType;
}

UINT32 TcpDispatcherInitializer::getConnectionId()
{
  return m_connectionId;
}

void TcpDispatcherInitializer::getRecivedDispatcherName(AnsiStringStorage *dispName)
{
  dispName->setString(m_gotDispatcherName.getString());
}
