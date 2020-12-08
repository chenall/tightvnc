// Copyright (C) 2010,2011,2012 GlavSoft LLC.
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

#ifndef __LOGCONN_H__
#define __LOGCONN_H__

#include "io-lib/Channel.h"
#include "thread/Thread.h"
#include "win-system/Pipe.h"
#include "LogListener.h"
#include "LogLevelSender.h"

class LogConnAuthListener;

class LogConn : public Thread
{
public:
  LogConn(Channel *channel, LogConnAuthListener *extAuthListener,
          LogListener *extLogListener, unsigned char logLevel);
  virtual ~LogConn();

  // This function closes the connection that brings to object
  // termination.
  virtual void close();

  void changeLogLevel(unsigned char newLevel);

private:
  virtual void execute();
  virtual void onTerminate();

  void assignConnection();
  void dispatch();

  LogConnAuthListener *m_extAuthListener;
  LogListener *m_extLogListener;

  Channel *m_serviceChannel;
  Channel *m_logListenChannel;
  Channel *m_levelSendChannel;
  FileAccountHandle m_handle;
  LocalMutex m_channelMutex;

  unsigned char m_logLevel;
  LocalMutex m_logLevelMutex;

  LogLevelSender m_logLevelSender;
};

#endif // __LOGCONN_H__
