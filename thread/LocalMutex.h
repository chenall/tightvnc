// Copyright (C) 2008,2009,2010,2011,2012 GlavSoft LLC.
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

#ifndef __LOCALMUTEX_H__
#define __LOCALMUTEX_H__

#include "util/CommonHeader.h"

#include "Lockable.h"

/**
 * Local mutex (cannot be used within separate processes).
 *
 * @remark local mutex uses Windows critical sections to implement
 * lockable interface..
 */
class LocalMutex : public Lockable
{
public:
  /**
   * Creates new local mutex.
   */
  LocalMutex();

  /**
   * Deletes local mutex.
   */
  virtual ~LocalMutex();

  /**
   * Inherited from Lockable.
   */
  virtual void lock();

  /**
   * Inherited from Lockable.
   */
  virtual void unlock();

private:
  /**
   * Windows critical section.
   */
  CRITICAL_SECTION m_criticalSection;
};

#endif // __LOCALMUTEX_H__
