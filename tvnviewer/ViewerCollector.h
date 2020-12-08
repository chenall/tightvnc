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

#ifndef _VIEWER_COLLECTOR_H_
#define _VIEWER_COLLECTOR_H_

#include "thread/LocalMutex.h"
#include <list>
#include "ViewerInstance.h"

typedef std::list<ViewerInstance *> InstanceList;

// Collector instances.
class ViewerCollector
{
public:
  ViewerCollector();
  virtual ~ViewerCollector();

  // Adds instance to a self list.
  void addInstance(ViewerInstance *instance);

  // Forces terminates all instances, waits until they dies and than
  // delete them from memory and thread list.
  void destroyAllInstances();

  // This function decrease counter of connection, requires reconnect.
  void decreaseToReconnect();

  // Return true, if and only if m_instances is empty()
  bool empty() const;

  // Deletes all stopped instances from memory and removes them from self list.
  void deleteDeadInstances();

protected:
  mutable LocalMutex m_lockObj;
  InstanceList m_instances;

  // This variable contain count of instance, when need to reconnect.
  // If this count isn't 0, shutdown application is denied.
  int m_countToReconnect;
};

#endif
