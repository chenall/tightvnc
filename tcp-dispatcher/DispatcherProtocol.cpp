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

#include "DispatcherProtocol.h"

BadDispatcherProtocolException::BadDispatcherProtocolException(const TCHAR *message)
: Exception(message)
{
}

DispatcherProtocol::DispatcherProtocol(RfbInputGate *inputGate,
                                       RfbOutputGate *outputGate,
                                       LogWriter *log)
: m_input(inputGate),
  m_output(outputGate),
  m_log(log)
{
}

DispatcherProtocol::DispatcherProtocol(RfbInputGate *inputGate,
                                       RfbOutputGate *outputGate,
                                       const char *sendDispatcherName,
                                       bool connectionType,
                                       UINT32 connectionId,
                                       const char *keyword,
                                       LogWriter *log)
: m_input(inputGate),
  m_output(outputGate),
  m_log(log),
  m_sendDispatcherName(sendDispatcherName),
  m_connectionType(connectionType),
  m_connectionId(connectionId),
  m_keyword(keyword)
{
}

DispatcherProtocol::~DispatcherProtocol()
{
}

void DispatcherProtocol::init(const char *sendDispatcherName,
                              bool connectionType,
                              UINT32 connectionId,
                              const char *keyword)
{
  m_sendDispatcherName.setString(sendDispatcherName);
  m_connectionType = connectionType;
  m_connectionId = connectionId;
  m_keyword.setString(keyword);
}

bool DispatcherProtocol::checkProtocolSignature(const char str[13])
{
  bool correctProtocol = (strcmp(str, "TCPDISPATCH\n") == 0);
  return correctProtocol;
}

void DispatcherProtocol::readProtocolType()
{
  char charBuf[13];
  m_input->readFully(charBuf, 12); // Read "TCPDISPATCH\n"
  charBuf[13] = '\0';

  if (!checkProtocolSignature(charBuf)) {
    throw BadDispatcherProtocolException(_T("Unknown Dispatcher protocol"));
  }
}

void DispatcherProtocol::continueTcpDispatchProtocol()
{
  negotiateProtocolVersion();
  sendConnectionType();
  negotiateConnectionId();
  sendKeyword();
  exchangeDispatcherNames();
}

void DispatcherProtocol::negotiateProtocolVersion()
{
  // Get supported versions
  UINT8 numSupportedVersion = m_input->readUInt8();
  if (numSupportedVersion == 0) {
    StringStorage errMess;
    errMess.format(_T("Illegal number of supported version: %d"),
                   (int)numSupportedVersion);
    throw Exception(errMess.getString());
  }
  std::vector<UINT8> supportedVersion(numSupportedVersion);
  m_input->readFully(&supportedVersion.front(), supportedVersion.size());

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
  m_output->writeUInt8(effectiveVersion);
  m_output->flush();
}

void DispatcherProtocol::sendConnectionType()
{
  // Send connection type (0 = RfbServer)
  m_output->writeUInt8((UINT8)m_connectionType);
  m_output->flush();
}

void DispatcherProtocol::negotiateConnectionId()
{
  m_output->writeUInt32(m_connectionId);
  m_output->flush();
  m_log->info(_T("sent connection id = %u"), m_connectionId);
  if (m_connectionId == 0) {
    // Get connection Id
    m_connectionId = m_input->readUInt32();
    m_log->info(_T("Got connection id = %u"), m_connectionId);
  }
}

void DispatcherProtocol::sendKeyword()
{
  size_t length = m_keyword.getLength();
  if (length > 255) {
    length = 255;
  }
  m_output->writeUInt8((UINT8)length);
  m_output->writeFully(m_keyword.getString(), length);
  m_output->flush();
}

void DispatcherProtocol::exchangeDispatcherNames()
{
  // Send a predefined dispatcher name
  size_t length = m_sendDispatcherName.getLength();
  if (length > 255) {
    length = 255;
  }
  m_output->writeUInt8((UINT8)length);
  m_output->writeFully(m_sendDispatcherName.getString(), length);
  m_output->flush();

  // Get a dispatcher name from the dispatcher.
  length = m_input->readUInt8();
  std::vector<char> gotName(length + 1);
  m_input->readFully(&gotName.front(), length);
  gotName[length] = 0;
  m_gotDispatcherName.setString(&gotName.front());
}

void DispatcherProtocol::waitNextProtocolContinue()
{
  // Wait until get an allowing byte
  while (m_input->readUInt8() == 0) {
    m_output->writeUInt8(0);
    m_output->flush();
  }
}

UINT32 DispatcherProtocol::getConnectionId()
{
  return m_connectionId;
}

void DispatcherProtocol::getRecivedDispatcherName(AnsiStringStorage *dispName)
{
  dispName->setString(m_gotDispatcherName.getString());
}
