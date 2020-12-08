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

#ifndef _LOGGER_H_
#define _LOGGER_H_

#include "util/CharDefs.h"

//
// The Logger class defines abstract low-level interface for logging
// (recording various types of events in software components or applications).
//
// To develop an implementation (e.g. a logger which writes to a file, to the
// console etc.), inherit from this class and override its print() function.
// For example, see FileLogger which implements the Logger interface to log
// into a file.
//
// Normally, this class and its subclasses should not be used directly. It's
// recommended to use LogWriter which adds separate functions for different
// log verbosity levels, and adds message formatting.
//
// When developing components and libraries that use logging but should stay
// independent from the actual logging method, allow passing Logger* pointer
// to your module and construct LogWriter wrapper around that Logger. That
// allows your caller to pass any sub-class of the Logger to your component,
// or operate without logging by passing a null pointer.
//
class Logger
{
public:
  //
  // Implementations of this abstract function should process a log event (for
  // example, print it, send it somewhere, or record it in some type of
  // storage).
  //
  // The line argument is the string to be logged.
  //
  // The logLevel argument defines significance of the event. Higher values of
  // logLevel correspond to less important events. The recommended range for
  // logLevel is 0..9. When this function is called, a Logger implementation
  // should check logLevel and decide either to accept or to decline the log
  // message.
  //
  virtual void print(int logLevel, const TCHAR *line) = 0;

  //
  // Implementations of this abstract function should return true if they ready
  // to process a message with this logLevel. That is you can test messages for
  // for accepting or declining before calling the print() function.
  //
  virtual bool acceptsLevel(int logLevel) = 0;
};

#endif // _LOGGER_H_
