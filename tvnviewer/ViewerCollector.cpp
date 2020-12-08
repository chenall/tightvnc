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

#include "ViewerCollector.h"
#include "thread/AutoLock.h"

ViewerCollector::ViewerCollector()
: m_countToReconnect(0)
{
}

ViewerCollector::~ViewerCollector()
{
  destroyAllInstances();
}

void ViewerCollector::addInstance(ViewerInstance *viewerInstance)
{
  AutoLock l(&m_lockObj);
  m_instances.push_back(viewerInstance);
}

void ViewerCollector::deleteDeadInstances()
{
  AutoLock l(&m_lockObj);

  InstanceList::iterator iter = m_instances.begin();
  while (iter != m_instances.end()) {
    ViewerInstance *instance = *iter;
    if (instance->isStopped()) {
      if (instance->requiresReconnect()) {
        ++m_countToReconnect;
      }
      delete instance;
      iter = m_instances.erase(iter);
    } else {
      iter++;
    }
  }
}

void ViewerCollector::destroyAllInstances()
{
  AutoLock l(&m_lockObj);

  InstanceList::iterator iter;
  for (iter = m_instances.begin(); iter != m_instances.end(); iter++) {
    (*iter)->stop();
  }
  for (iter = m_instances.begin(); iter != m_instances.end(); iter++) {
    (*iter)->waitViewer();
  }

  deleteDeadInstances();
}

void ViewerCollector::decreaseToReconnect()
{
  AutoLock l(&m_lockObj);

  --m_countToReconnect;
}

bool ViewerCollector::empty() const
{
  AutoLock l(&m_lockObj);

  // If not active instance and count wait to reconnect is 0,
  // then return true and false otherwise.
  return m_instances.empty() && (m_countToReconnect == 0);
}
