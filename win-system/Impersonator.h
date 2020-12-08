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

#ifndef _IMPERSONATOR_H_
#define _IMPERSONATOR_H_

#include "util/CommonHeader.h"
#include "util/StringStorage.h"
#include "log-writer/LogWriter.h"

#include "SystemException.h"

/**
Class for impersonating current process as user that logged on current
console session.
@remark: can work only in XP and later cause it uses WTSQueryUserToken function.
*/
class Impersonator
{
public:
  Impersonator(LogWriter *log);
  virtual ~Impersonator();

  /**
  Impersonates calling process as user that logged on current console session.
  @throws SystemException if impersonation fails.
  */
  virtual void impersonateAsLoggedUser() throw(SystemException);

  /**
  Cancels effect of impersonateAsLoggedUser method call.
  @throws SystemException on fail.
  */
  virtual void revertToSelf() throw(SystemException);

protected:
  HANDLE m_token;
  HANDLE m_dupToken;

  LogWriter *m_log;
};

#endif
