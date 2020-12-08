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

#ifndef _HTTP_REQUEST_H_
#define _HTTP_REQUEST_H_

#include "io-lib/DataInputStream.h"

#include "ArgList.h"

class HttpRequest
{
public:
  HttpRequest(DataInputStream *dataInput);
  virtual ~HttpRequest();

  // Reads HTTP header.
  void readHeader() throw(IOException);

  // Parse read header.
  // Returns true if header is valid, false otherwise.
  // Remark: method must be called after readHeader().
  bool parseHeader();

  // Returns read request.
  const char *getRequest() const;
  // Returns requested filename.
  const char *getFilename() const;
  // Return true if request has at least one argument specified.
  bool hasArguments() const;
  // Returns request arguments container.
  ArgList *getArguments() const;

protected:
  // Skips HTTP headers until end.
  void skipHeader(bool lastWasEndLn) throw(IOException);
  // Reads line that ends with specified character from data input stream
  // and storage it output buffer parameter.
  // If line is more than specified max size that string will be trunkated to
  // maxSize.
  void readLine(char endLnChar, char *buffer, size_t maxSize) throw(IOException);

protected:
  static const size_t REQUEST_BUFFER_SIZE = 2048;

protected:
  // Stream for reading data.
  DataInputStream *m_dataInput;
  // Read request.
  char m_request[REQUEST_BUFFER_SIZE];
  // Requested filename.
  char m_filename[REQUEST_BUFFER_SIZE];
  // Arguments string.
  char m_args[REQUEST_BUFFER_SIZE];
  // Arguments list.
  ArgList *m_argList;
};

#endif
