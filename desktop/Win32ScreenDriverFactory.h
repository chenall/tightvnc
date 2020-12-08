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

#ifndef __WIN32SCREENDRIVERFACTORY_H__
#define __WIN32SCREENDRIVERFACTORY_H__

#include "ScreenDriverFactory.h"
#include "server-config-lib/ServerConfig.h"

class Win32ScreenDriverFactory : public ScreenDriverFactory
{
public:
  // The factory stores pointer to a ServerConfig object and will use it on
  // createScreenDriver() function calls.
  Win32ScreenDriverFactory(ServerConfig *srvConf);
  virtual ~Win32ScreenDriverFactory();

  virtual ScreenDriver *createScreenDriver(UpdateKeeper *updateKeeper,
                                             UpdateListener *updateListener,
                                             FrameBuffer *fb,
                                             LocalMutex *fbLocalMutex,
                                             LogWriter *log);
private:
  ScreenDriver *createStandardScreenDriver(UpdateKeeper *updateKeeper,
                                             UpdateListener *updateListener,
                                             FrameBuffer *fb,
                                             LocalMutex *fbLocalMutex,
                                             LogWriter *log);
  ScreenDriver *createMirrorScreenDriver(UpdateKeeper *updateKeeper,
                                           UpdateListener *updateListener,
                                           LocalMutex *fbLocalMutex,
                                           LogWriter *log);

  bool isMirrorDriverAllowed();
  bool isD3DAllowed();

  ServerConfig *m_srvConf;
};

#endif // __WIN32SCREENDRIVERFACTORY_H__
