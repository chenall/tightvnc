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

#ifndef __RECONNECTINGCHANNEL_H__
#define __RECONNECTINGCHANNEL_H__

#include "io-lib/Channel.h"
#include "thread/LocalMutex.h"
#include "win-system/WindowsEvent.h"
#include "log-writer/LogWriter.h"

class ReconnectingChannel : public Channel
{
public:
  ReconnectingChannel(unsigned int timeOut, LogWriter *log);
  virtual ~ReconnectingChannel();

  virtual size_t read(void *buffer, size_t len);
  virtual size_t write(const void *buffer, size_t len);

  // Replaces invalid channel by the new valid.
  // At the next the read()/write() function call read()/write()
  // generates the ReconnectException if this is not the first initialization
  void replaceChannel(Channel *newChannel);

  // Closes connection and break all blocked operation.
  // @throw Exception on error.
  virtual void close();

  virtual size_t available() { return 0; };

private:
  // @param funName - is a function name that will be placed to the
  // ReconnectException text.
  // @throw ReconnectException on reconnect detection.
  // @throw IOException on other errors.
  Channel *getChannel(const TCHAR *funName);

  // @param funName - is a function name that will be placed to the
  // ReconnectException text.
  // @param channel - currently using transport.
  // @throw ReconnectException on reconnect detection.
  // @throw IOException on other errors.
  void waitForReconnect(const TCHAR *funName, Channel *channel);

  bool m_isClosed;

  Channel *m_channel;
  Channel *m_oldChannel;
  bool m_chanWasChanged;
  LocalMutex m_chanMut;

  WindowsEvent m_timer;
  unsigned int m_timeOut;

  LogWriter *m_log;
};

#endif // __RECONNECTINGCHANNEL_H__
