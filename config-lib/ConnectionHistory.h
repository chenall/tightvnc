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

#ifndef _CONNECTION_HISTORY_H_
#define _CONNECTION_HISTORY_H_

#include "win-system/RegistryKey.h"

#include <vector>

using namespace std;

/**
 * History of tcp connections.
 *
 * Solves problem with saving and restoring history of connections
 * for user (UI).
 *
 */
class ConnectionHistory
{
public:
  /**
   * Creates new empty connection history.
   * @param key registry key identifing root storage for connection history.
   * @param limit maximum count of connection history entries.
   */
  ConnectionHistory(RegistryKey *key, size_t limit);
  /**
   * Destructor.
   */
  virtual ~ConnectionHistory();

  /**
   * Sets connection history entries limit and truncates history
   * if needed.
   */
  void setLimit(size_t limit);

  /**
   * Returns connection history entries limit.
   */
  size_t getLimit() const;

  /**
   * Loads connection history from registry.
   */
  void load();

  /**
   * Saves connection history to registry.
   */
  void save();

  /**
   * Truncates connection history (memory and registry both) to max limit.
   */
  void truncate();

  /**
   * Clears connection history and removed it from registry.
   */
  void clear();

  /**
   * Adds host to connection history.
   * @param host host to add to history.
   * @remark: if host is already in connection history,
     then it's just moves it up to first position.
   */
  void addHost(const TCHAR *host);

  /**
   * Return hosts count.
   * @return hosts count.
   */
  size_t getHostCount() const;

  /**
   * Return host with specified index.
   * @param i index of history entry.
   * @return host string.
   */
  const TCHAR *getHost(size_t i) const;

protected:
  void releaseHosts();
  void removeHost(const TCHAR *host);

protected:
  /**
   * Array of history enries.
   */
  vector<StringStorage> m_hosts;

  /**
   * Maximum count of history entries.
   */
  size_t m_limit;

  /**
   * Registry key for saving and loading connection history data.
   */
  RegistryKey *m_key;
};

#endif
