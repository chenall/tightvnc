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

#ifndef _OPERATION_EVENT_LISTENER_H_
#define _OPERATION_EVENT_LISTENER_H_

#include "FileTransferOperation.h"

class OperationEventListener
{
public:
  OperationEventListener();
  ~OperationEventListener();

  //
  // Must be called by file transfer operation when operation had started
  //

  virtual void ftOpStarted(FileTransferOperation *sender);

  //
  // Must be called by file transfer operation when operation had finished
  //

  virtual void ftOpFinished(FileTransferOperation *sender);

  //
  // Must be called by file transfer operation when some error occured(
  // not list files from catalog, cannot open file, cannot create folder etc)
  //

  virtual void ftOpErrorMessage(FileTransferOperation *sender, const TCHAR *message);

  //
  // Must be called by file transfer operation when need to do some text output
  // to user (or console, or log)
  //

  virtual void ftOpInfoMessage(FileTransferOperation *sender, const TCHAR *message);
};

#endif

