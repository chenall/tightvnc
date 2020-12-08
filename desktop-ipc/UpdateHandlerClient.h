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

#ifndef __UPDATEHANDLERCLIENT_H__
#define __UPDATEHANDLERCLIENT_H__

#include "desktop/UpdateHandler.h"
#include "DesktopServerProto.h"
#include "DesktopSrvDispatcher.h"
#include "log-writer/LogWriter.h"

class UpdateHandlerClient : public UpdateHandler, public DesktopServerProto,
                            public ClientListener
{
public:
  UpdateHandlerClient(BlockingGate *forwGate, DesktopSrvDispatcher *dispatcher,
                      UpdateListener *externalUpdateListener, LogWriter *log);
  virtual ~UpdateHandlerClient();

  virtual void extract(UpdateContainer *updateContainer);
  virtual void setFullUpdateRequested(const Region *region);
  virtual void setExcludedRegion(const Region *excludedRegion);
  virtual bool checkForUpdates(Region *region);

protected:
  virtual void getScreenProperties(PixelFormat *pf, Dimension *dim);
  virtual void sendInit(BlockingGate *gate);

  // To catch update event
  virtual void onRequest(UINT8 reqCode, BlockingGate *backGate);

  UpdateListener *m_externalUpdateListener;

  LogWriter *m_log;
};

#endif // __UPDATEHANDLERCLIENT_H__
