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

#ifndef _OPERATION_NOT_PERMITTED_EXECEPTIONH_H_
#define _OPERATION_NOT_PERMITTED_EXECEPTIONH_H_

#include "util/Exception.h"

//
// This exception is raised when client receives valid (from ft protocol extension
// point of view) rfb message, but it's invalid from file transfer client logic.
//
// For example:
//
// Client is removing files, it's waiting for the rmReply or lastRequestFailed,
// but rfb message that comes from server is downloadDataReply or uploadDataReply or
// something that can't be received in current client state.
//

class OperationNotPermittedException : public Exception
{
public:
  OperationNotPermittedException();
  virtual ~OperationNotPermittedException();
};

#endif
