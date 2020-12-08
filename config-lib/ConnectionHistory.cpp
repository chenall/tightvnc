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

#include "ConnectionHistory.h"

#include "win-system/Registry.h"

#include <crtdbg.h>

ConnectionHistory::ConnectionHistory(RegistryKey *key, size_t limit)
: m_key(key), m_limit(limit)
{
}

ConnectionHistory::~ConnectionHistory()
{
  releaseHosts();
}

void ConnectionHistory::setLimit(size_t limit)
{
  bool truncationNeeded = limit < m_limit;

  m_limit = limit;

  if (truncationNeeded) {
    truncate();
  }
}

size_t ConnectionHistory::getLimit() const
{
  return m_limit;
}

void ConnectionHistory::load()
{
  releaseHosts();

  StringStorage valueName;
  StringStorage value;

  for (size_t i = 0; i < m_limit; i++) {
    valueName.format(_T("%d"), i);
    if (!m_key->getValueAsString(valueName.getString(), &value)) {
      break;
    }
    m_hosts.push_back(value);
  }
}

void ConnectionHistory::save()
{
  StringStorage valueName;

  size_t count = m_hosts.size();

  for (size_t i = 0; i < min(count, m_limit); i++) {
    valueName.format(_T("%u"), i);
    const TCHAR *value = m_hosts.at(i).getString();

    m_key->setValueAsString(valueName.getString(), value);
  }

  if (count > m_limit) {
    truncate();
  }
}

void ConnectionHistory::truncate()
{
  StringStorage valueName;
  StringStorage value;

  size_t i = (size_t)m_limit;

  while (true) {
    valueName.format(_T("%u"), i);

    if (i >= getHostCount()) {
      return ;
    }

    removeHost(getHost(i));

    if (!m_key->getValueAsString(valueName.getString(), &value)) {
      break;
    }

    m_key->deleteSubKey(value.getString());
    m_key->deleteValue(valueName.getString());

    i++;
  }

  load();
}

void ConnectionHistory::clear()
{
  StringStorage valueName;

  for (size_t i = 0; i < m_hosts.size(); i++) {
    valueName.format(_T("%u"), i);

    m_key->deleteSubKey(m_hosts.at(i).getString());
    m_key->deleteValue(valueName.getString());
  }

  releaseHosts();
}

void ConnectionHistory::addHost(const TCHAR *host)
{
  StringStorage hostS(host);

  for (vector<StringStorage>::iterator it = m_hosts.begin(); it != m_hosts.end(); it++) {
    if (it->isEqualTo(&hostS)) {
      m_hosts.erase(it);
      break;
    }
  }

  m_hosts.insert(m_hosts.begin(), hostS);
}

size_t ConnectionHistory::getHostCount() const
{
  return m_hosts.size();
}

const TCHAR *ConnectionHistory::getHost(size_t i) const
{
  return m_hosts.at(i).getString();
}

void ConnectionHistory::releaseHosts()
{
  m_hosts.clear();
}

void ConnectionHistory::removeHost(const TCHAR *host)
{
  StringStorage hostS(host);

  for (vector<StringStorage>::iterator it = m_hosts.begin(); it != m_hosts.end(); it++) {
    if (it->isEqualTo(&hostS)) {
      m_hosts.erase(it);
      break;
    }
  }
}
