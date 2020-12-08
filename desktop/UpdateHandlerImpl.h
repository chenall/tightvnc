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

#ifndef __UPDATEHANDLERIMPL_H__
#define __UPDATEHANDLERIMPL_H__

#include "UpdateHandler.h"
#include "ScreenDriver.h"
#include "ScreenDriverFactory.h"

// This class contain a base architecture implementation of the UpdateHandler class.
class UpdateHandlerImpl : public UpdateHandler, public UpdateListener
{
public:
  UpdateHandlerImpl(UpdateListener *externalUpdateListener, ScreenDriverFactory *scrDriverFactory,
                    LogWriter *log);
  virtual ~UpdateHandlerImpl();

  virtual void extract(UpdateContainer *updateContainer);
  virtual void setFullUpdateRequested(const Region *region);
  bool checkForUpdates(Region *region);

  virtual void setExcludedRegion(const Region *excludedRegion);

private:
  virtual void executeDetectors();
  virtual void terminateDetectors();

  void doUpdate()
  {
    if (m_externalUpdateListener) {
      m_externalUpdateListener->onUpdate();
    }
  }

  virtual void onUpdate();

  void applyNewScreenProperties();

  UpdateKeeper m_updateKeeper;
  ScreenDriver *m_screenDriver;
  UpdateFilter *m_updateFilter;
  UpdateListener *m_externalUpdateListener;

  Rect m_absoluteRect;

  LogWriter *m_log;

  bool m_fullUpdateRequested;
};

#endif // __UPDATEHANDLERIMPL_H__
