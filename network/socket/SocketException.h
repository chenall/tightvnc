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

#ifndef SOCKET_EXCEPTION_H
#define SOCKET_EXCEPTION_H

#include "util/Exception.h"

#include "sockdefs.h"

class SocketException : public Exception
{
public:
  // Creates socket exception object with error value eq to current value
  // of WSAGetLastError().
  // Error string generates automaticly;
  SocketException();
  // Creates socket exception object with specified error numner.
  // Error string generates automaticly;
  SocketException(int error);
  // Creates socket exception object with specified message.
  // Error number is sets to zero.
  SocketException(const TCHAR *message);
  // Destructor.
  virtual ~SocketException();

  // Returns error number.
  int errno();

protected:
  // Sets m_errno and generates error description string.
  void setErrno(int error);

protected:
  int m_errno;
};

#endif
