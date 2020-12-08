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

#ifndef _FT_SECURITY_H_
#define _FT_SECURITY_H_

#include "util/Exception.h"
#include "util/Singleton.h"
#include "log-writer/LogWriter.h"

#include "win-system/Impersonator.h"

#include "desktop/Desktop.h"

class FileTransferSecurity : private Impersonator
{
public:
  FileTransferSecurity(Desktop *desktop, LogWriter *log);
  virtual ~FileTransferSecurity();

  // Sets access rights for calling process for execution
  // file transfer request handler code.
  void beginMessageProcessing();
  // Checks result and throws exception if caller
  // must not execute file transfer code and return error
  // to client.
  void throwIfAccessDenied() throw(Exception);
  // Sets previous (before startMessageProcessing call) access rights
  // for calling process.
  void endMessageProcessing();

protected:
  bool m_hasAccess;

  Desktop *m_desktop;

  LogWriter *m_log;
};

#endif
