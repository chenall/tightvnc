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

#ifndef __UPDATEKEEPER_H__
#define __UPDATEKEEPER_H__

#include "thread/LocalMutex.h"
#include "region/Region.h"
#include "UpdateContainer.h"
#include "thread/AutoLock.h"

class UpdateKeeper : public Lockable
{
public:
  UpdateKeeper();
  UpdateKeeper(const Rect *borderRect);
  ~UpdateKeeper(void);

  virtual void lock()
  {
    m_updContLocMut.lock();
  }

  virtual void unlock()
  {
    m_updContLocMut.unlock();
  }

  void addChangedRegion(const Region *changedRegion);
  void addChangedRect(const Rect *changedRect);
  // Adds border rectangle to changed region.
  void dazzleChangedReg()
  {
    AutoLock al(&m_updContLocMut);
    addChangedRect(&m_borderRect);
  }

  void addCopyRect(const Rect *copyRect, const Point *src);

  void setBorderRect(const Rect *borderRect);

  void setScreenSizeChanged();
  void setCursorPosChanged(const Point *curPos);
  void setCursorPos(const Point *curPos);
  void setCursorShapeChanged();

  void setExcludedRegion(const Region *excludedRegion);

  void addUpdateContainer(const UpdateContainer *updateContainer);
  void getUpdateContainer(UpdateContainer *updCont);
  bool checkForUpdates(const Region *region);

  void extract(UpdateContainer *updateContainer);

private:
  Rect m_borderRect;

  Region m_excludedRegion;
  LocalMutex m_exclRegLocMut;

  UpdateContainer m_updateContainer;
  LocalMutex m_updContLocMut;
};

#endif // __UPDATEKEEPER_H__
