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

#ifndef _ZOMBIE_KILLER_H_
#define _ZOMBIE_KILLER_H_

#include "Thread.h"
#include "LocalMutex.h"

#include "util/Singleton.h"

#include <list>

using namespace std;

typedef list<Thread *> ThreadList;

/**
 * Collector of "zombie" threads.
 * Zombie killer has it's own thread which deletes in infinity loop not
 * active zombie threads.
 * @remark ZombieKiller is singleton without "lazy" initialization.
 */
class ZombieKiller : private Thread, public Singleton<ZombieKiller>
{
public:
  /**
   * Creates zombie killed instance.
   */
  ZombieKiller();
  /**
   * Deletes zombie killer.
   */
  virtual ~ZombieKiller();

  /**
   * Adds thread to zombie list.
   */
  void addZombie(Thread *zombie);

  /**
   * Forces terminates all threads, waits until they dies and than
   * delete them from memory and thread list.
   */
  void killAllZombies();

protected:
  /**
   * Inherited from Thread class.
   */
  virtual void execute();

  /**
   * Deletes all dead zombie threads from memory and removes them from zombies list.
   */
  void deleteDeadZombies();

protected:
  /**
   * Thread vector.
   */
  ThreadList m_zombies;

  /**
   * Thread-safety.
   */
  LocalMutex m_lockObj;
};

#endif
