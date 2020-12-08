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

#ifndef __GLOBALMUTEX_H__
#define __GLOBALMUTEX_H__

#include "util/CommonHeader.h"
#include "util/Exception.h"
#include "Lockable.h"

/**
 * Global mutex (allows to use mutex between separate processes).
 *
 * @author yuri, enikey.
 */
class GlobalMutex : public Lockable
{
public:
  /**
   * Creates new global mutex.
   * @param [optional] name name of mutex.
   * @param throwIfExsist if flag is set then thows exception if mutex exsists.
   * @param interSession if set, then mutex can be accessed from separate sessions, if not,
   * then every session will create it's own mutex.
   * @remark if name is 0, then mutex will be unnamed.
   * @throws Exception when cannot create mutex or when throwIfExist flag is set
   * and mutex already exist.
   */
  GlobalMutex(const TCHAR *name = 0, bool interSession = false, bool throwIfExist = false) throw(Exception);

  /**
   * Deletes global mutex.
   */
  virtual ~GlobalMutex();

  /**
   * Inherited from Lockable.
   */
  virtual void lock();

  /**
   * Inherited from Lockable.
   */
  virtual void unlock();

private:
  void setAccessToAll(HANDLE objHandle);

  HANDLE m_mutex;
};

#endif // __GLOBALMUTEX_H__
