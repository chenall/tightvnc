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

#include <string.h>
#include <stdlib.h>
#include "HostPath.h"

const size_t HostPath::m_SSH_USER_MAX_CHARS = 32;
const size_t HostPath::m_SSH_HOST_MAX_CHARS = 255;
const size_t HostPath::m_SSH_PORT_MAX_CHARS = 5;
const size_t HostPath::m_VNC_HOST_MAX_CHARS = 255;
const size_t HostPath::m_VNC_PORT_MAX_CHARS = 5;
// Compute the maximal length of a valid path. Note that there can be no
// more than five delimiter characters: "user@sshhost:port/vnchost::port".
const size_t HostPath::m_MAX_PATH_LEN =
(HostPath::m_SSH_USER_MAX_CHARS + HostPath::m_SSH_HOST_MAX_CHARS + HostPath::m_SSH_PORT_MAX_CHARS +
  HostPath::m_VNC_HOST_MAX_CHARS + HostPath::m_VNC_PORT_MAX_CHARS + 5);

HostPath::HostPath()
  : m_path(0),
    m_sshHost(0),
    m_sshPort(0),
    m_vncHost(0),
    m_vncPort(0),
    m_defaultPort(5900)
{
}

HostPath::HostPath(const char *path, int defaultPort)
  : m_path(0),
    m_sshHost(0),
    m_sshPort(0),
    m_vncHost(0),
    m_vncPort(0),
    m_defaultPort(defaultPort)
{
  set(path);
}

HostPath::~HostPath()
{
  clear();
}

bool
HostPath::set(const char *path)
{
  // Forget previous path if one was set earlier.
  clear();

  // Check the path length and save a copy for this object.
  if (path == 0) {
    return false;
  }
  size_t pathLen = strnlen(path, m_MAX_PATH_LEN);
  if (pathLen < 1 || pathLen > m_MAX_PATH_LEN) {
    return false;
  }
  m_path = new char[pathLen + 1];
  memcpy(m_path, path, pathLen);
  m_path[pathLen] = '\0';

  // Perform initial parsing and checking of the path.
  size_t tokens[4];
  parsePath(tokens);
  if ( tokens[0] + tokens[2] + tokens[3] == 0 ||
       (tokens[1] != 0 && tokens[0] == 0) ) {
    clear();
    return false;
  }
  const char *tokenStart = m_path;

  // Handle SSH host name.
  if (tokens[0] != 0) {
    size_t hostLen = tokens[0];
    if (hostLen > m_SSH_HOST_MAX_CHARS) {
      clear();
      return false;
    }
    m_sshHost = new char[hostLen + 1];
    memcpy(m_sshHost, tokenStart, hostLen);
    m_sshHost[hostLen] = '\0';
    m_sshPort = 22;
    tokenStart += tokens[0];

    // Handle SSH port number.
    if (tokens[1] != 0) {
      size_t portLen = tokens[1] - 1;
      if (portLen < 1 ||
          portLen > m_SSH_PORT_MAX_CHARS ||
          *tokenStart != ':' ||
          strspn(tokenStart + 1, "0123456789") != portLen) {
        clear();
        return false;
      }
      m_sshPort = atoi(tokenStart + 1);
      tokenStart += tokens[1];
    }

    tokenStart++; // skip '/'
  }

  // Handle VNC host name.
  const char* hostStart = tokenStart;
  size_t hostLen = tokens[2];
  if (tokens[2] == 0) {
    hostStart = "localhost";
    hostLen = 9; // strlen("localhost")
  } else {
    if (hostLen > m_VNC_HOST_MAX_CHARS) {
      clear();
      return false;
    }
  }
  m_vncHost = new char[hostLen + 1];
  memcpy(m_vncHost, hostStart, hostLen);
  m_vncHost[hostLen] = '\0';
  tokenStart += tokens[2];

  // Handle VNC display or port number.
  if (tokens[3] == 0) {
    m_vncPort = m_defaultPort;
  } else {
    size_t portLen = tokens[3] - 1;
    if (portLen < 1 || *tokenStart != ':') {
      clear();
      return false;
    }
    const char* portStart = tokenStart + 1;
    bool twoColons = (*portStart == ':');
    if (twoColons) {
      portStart++;
      portLen--;
    }
    if (portLen < 1 ||
        portLen > m_VNC_PORT_MAX_CHARS ||
        strspn(portStart, "0123456789") != portLen) {
      clear();
      return false;
    }
    m_vncPort = atoi(portStart);
    if (!twoColons && m_vncPort >= 0 && m_vncPort <= 99) {
      m_vncPort += m_defaultPort;
    }
  }

  // Perform strict validation of m_vncHost (and m_sshHost if present).
  if (!validateHostNames()) {
    clear();
    return false;
  }

  return true;
}

void
HostPath::clear()
{
  if (m_path != 0) {
    delete[] m_path;
    m_path = 0;
  }
  if (m_sshHost != 0) {
    delete[] m_sshHost;
    m_sshHost = 0;
  }
  if (m_vncHost != 0) {
    delete[] m_vncHost;
    m_vncHost = 0;
  }
  m_sshPort = 0;
  m_vncPort = 0;
}

void
HostPath::parsePath(size_t results[]) const
{
  memset(results, 0, 4 * sizeof(size_t));

  if (m_path == 0)
    return;

  const char* vncHostStart = m_path;

  const char* slashPtr = strchr(m_path, '/');
  if (slashPtr != 0) {
    results[0] = strcspn(m_path, ":/");
    results[1] = (slashPtr - m_path) - results[0];
    vncHostStart = slashPtr + 1;
  }

  const char *colonPtr = strchr(vncHostStart, ':');
  if (colonPtr != 0) {
    results[2] = colonPtr - vncHostStart;
    results[3] = strnlen(colonPtr, m_MAX_PATH_LEN);
  } else {
    results[2] = strnlen(vncHostStart, m_MAX_PATH_LEN);
  }
}

bool
HostPath::validateHostNames() const
{
  const char* acceptChars =
    ".-_ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz01234567890";

  if (m_sshHost != 0) {
    const char* sshHostPtr = m_sshHost;
    size_t sshHostLen = strlen(m_sshHost);

    const char *sobakaPtr = strchr(m_sshHost, '@');
    if (sobakaPtr != 0) {
      const char* sshUserPtr = m_sshHost;
      size_t sshUserLen = sobakaPtr - m_sshHost;
      sshHostPtr = sobakaPtr + 1;
      sshHostLen = sshHostLen - sshUserLen - 1;

      if (sshUserLen < 1 || sshUserLen > m_SSH_USER_MAX_CHARS ||
          strspn(sshUserPtr, acceptChars) != sshUserLen) {
        return false;
      }
    }

    if (sshHostLen < 1 || sshHostLen > m_SSH_HOST_MAX_CHARS ||
        strspn(sshHostPtr, acceptChars) != sshHostLen) {
      return false;
    }
  }

  if (m_vncHost == 0)
    return false;

  size_t vncHostLen = strlen(m_vncHost);
  if (vncHostLen < 1 || vncHostLen > m_VNC_HOST_MAX_CHARS ||
      strspn(m_vncHost, acceptChars) != vncHostLen) {
    return false;
  }

  return true;
}
