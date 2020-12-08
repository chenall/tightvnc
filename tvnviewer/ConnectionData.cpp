// Copyright (C) 2012 GlavSoft LLC.
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

#include <iomanip>
#include <sstream>

#include "ConnectionData.h"
#include "util/AnsiStringStorage.h"
#include "util/VncPassCrypt.h"
#include "viewer-core/VncAuthentication.h"

ConnectionData::ConnectionData()
: m_isEmpty(true),
  m_isSetPassword(false),
  m_isSetDispatchId(false),
  m_isIncoming(false)
{
}

ConnectionData::ConnectionData(const ConnectionData &connectionData)
: m_isEmpty(connectionData.m_isEmpty),
  m_isSetPassword(connectionData.m_isSetPassword),
  m_isSetDispatchId(connectionData.m_isSetDispatchId),
  m_isIncoming(connectionData.m_isIncoming)
{
  if (!connectionData.isEmpty()) {
    m_hostPath.set(connectionData.m_hostPath.get());
  }
  if (m_isSetPassword) {
    m_defaultPassword = connectionData.m_defaultPassword;
  }
  if (m_isSetDispatchId) {
    m_dispatchId = connectionData.m_dispatchId;
  }
}

void ConnectionData::setIncoming(bool isIncoming)
{
  m_isIncoming = isIncoming;
}

bool ConnectionData::isIncoming() const
{
  return m_isIncoming;
}

bool ConnectionData::isEmpty() const
{
  return m_isEmpty;
}

bool ConnectionData::isSetPassword() const
{
  return m_isSetPassword;
}

void ConnectionData::resetPassword()
{
  m_isSetPassword = false;
}

UINT32 ConnectionData::getDispatchId() const
{
  return (m_isSetDispatchId ? m_dispatchId : 0);
}

bool ConnectionData::isSetDispatchId() const
{
  return m_isSetDispatchId;
}

void ConnectionData::setDispatchId(UINT32 id)
{
  m_dispatchId = id;
  m_isSetDispatchId = true;
}

void ConnectionData::unsetDispatchId()
{
  m_isSetDispatchId = false;
}

void ConnectionData::setHost(const StringStorage *host)
{
  StringStorage chompedString = *host;
  TCHAR spaceChar[] = _T(" \t\n\r");
  chompedString.removeChars(spaceChar, sizeof(spaceChar)/sizeof(TCHAR));

  AnsiStringStorage ansiStr(&chompedString);

  m_hostPath.set(ansiStr.getString());
  m_isEmpty = false;
}

StringStorage ConnectionData::getCryptedPassword() const
{
  return m_defaultPassword;
}

void ConnectionData::setCryptedPassword(const StringStorage *hidePassword)
{
  if (hidePassword == 0) {
    m_isSetPassword = false;
  } else {
    m_defaultPassword = *hidePassword;
    m_isSetPassword = true;
  }
}

StringStorage ConnectionData::getPlainPassword() const
{
  // Transform password from hex-string to raw data.
  AnsiStringStorage ansiHidePassword(&m_defaultPassword);
  UINT8 encPassword[VncAuthentication::VNC_PASSWORD_SIZE];
  for (size_t i = 0; i < VncAuthentication::VNC_PASSWORD_SIZE; ++i) {
    std::stringstream passwordStream;
    passwordStream << ansiHidePassword.getString()[i * 2]
                   << ansiHidePassword.getString()[i * 2 + 1];
    int ordOfSymbol = 0;
    passwordStream >> std::hex >> ordOfSymbol;
    encPassword[i] = static_cast<UINT8>(ordOfSymbol);
  }
  // Decrypt password.
  UINT8 plainPassword[VncAuthentication::VNC_PASSWORD_SIZE];
  VncPassCrypt::getPlainPass(plainPassword, encPassword);

  AnsiStringStorage ansiPlainPassword(reinterpret_cast<char *>(plainPassword));
  StringStorage password;
  ansiPlainPassword.toStringStorage(&password);
  return password;
}

void ConnectionData::setPlainPassword(const StringStorage *password)
{
  AnsiStringStorage ansiPlainPassword(password);
  UINT8 plainPassword[VncAuthentication::VNC_PASSWORD_SIZE];
  UINT8 encryptedPassword[VncAuthentication::VNC_PASSWORD_SIZE];
  memset(plainPassword, 0, VncAuthentication::VNC_PASSWORD_SIZE);
  memcpy(plainPassword,
         ansiPlainPassword.getString(),
         min(VncAuthentication::VNC_PASSWORD_SIZE, ansiPlainPassword.getSize()));
  VncPassCrypt::getEncryptedPass(encryptedPassword, plainPassword);
  UINT8 hidePasswordChars[VncAuthentication::VNC_PASSWORD_SIZE * 2 + 1];
  hidePasswordChars[VncAuthentication::VNC_PASSWORD_SIZE * 2] = 0;
  for (size_t i = 0; i < VncAuthentication::VNC_PASSWORD_SIZE; ++i) {
    std::stringstream passwordStream;
    int ordOfSymbol = encryptedPassword[i];
    passwordStream << std::hex << std::setw(2) << std::setfill('0') << ordOfSymbol;
    passwordStream >> hidePasswordChars[i * 2] >> hidePasswordChars[i * 2 + 1];
  }
  AnsiStringStorage ansiHidePassword(reinterpret_cast<char *>(hidePasswordChars));

  // save password
  ansiHidePassword.toStringStorage(&m_defaultPassword);
  m_isSetPassword = true;
}

StringStorage ConnectionData::getHost() const
{
  StringStorage host;
  AnsiStringStorage hostAnsi(m_hostPath.get());
  hostAnsi.toStringStorage(&host);
  return host;
}

void ConnectionData::getReducedHost(StringStorage *strHost) const
{
  AnsiStringStorage ansiStr(m_hostPath.getVncHost());
  ansiStr.toStringStorage(strHost);
}

int ConnectionData::getPort() const
{
  return m_hostPath.getVncPort();
}
