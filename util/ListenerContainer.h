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

#ifndef _LISTENER_CONTAINER_H_
#define _LISTENER_CONTAINER_H_

#include <vector>

#include "thread/LocalMutex.h"
#include "thread/AutoLock.h"

using namespace std;

template<class T> class SafeVector : public vector<T>,
                                     public LocalMutex {
public:
  virtual ~SafeVector() { };
};

//
// Template class that contains listeners array and methods
// for adding, removing listeners.
//

template<class T> class ListenerContainer {
public:

  //
  // Adds listener to class listeners list
  //

  void addListener(T listener) {
    AutoLock l(&m_listeners);

    vector<T>::iterator it;
    for (it = m_listeners.begin(); it != m_listeners.end(); it++) {
      T current = *it;
      if (current == listener) {
        return ;
      } // if found
    } // for all listeners
    m_listeners.push_back(listener);
  } // void

  void removeAllListeners() {
    AutoLock l(&m_listeners);

    m_listeners.clear();
  }

  //
  // Removes listener from class listeners list
  //

  void removeListener(T listener) {
    AutoLock l(&m_listeners);

    vector<T>::iterator it;
    for (it = m_listeners.begin(); it != m_listeners.end(); it++) {
      T current = *it;
      if (current == listener) {
        m_listeners.erase(it);
        return ;
      } // if found
    } // for all listeners
  } // void

protected:
  SafeVector<T> m_listeners;
};

#endif
