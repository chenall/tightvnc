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

#ifndef _SYSTEM_EXCEPTION_H_
#define _SYSTEM_EXCEPTION_H_

#include "util/Exception.h"

/**
 * Windows exception.
 *
 * Solves problem with generating formatted message strings width describes
 * user code-space where error occured and windows specific information about WinAPI error.
 */
class SystemException : public Exception
{
public:
  /**
   * Creates exception with formatted message from system
   * and error code which equals to GetLastError() value.
   */
  SystemException();
  /**
   * Creates exception with formatted message from system
   * and error code.
   * @param errcode windows error code.
   */
  SystemException(int errcode);
  /**
   * Creates exception with user message + formatted message from system
   * and error code set to GetLastError() value.
   * @param userMessage user message.
   */
  SystemException(const TCHAR *userMessage);
  /**
   * Creates exception with user message + formatted message from system
   * and specified error code.
   * @param userMessage user message.
   * @param errcode windows error code.
   */
  SystemException(const TCHAR *userMessage, int errcode);
  /**
   * Destructor, does nothing.
   */
  virtual ~SystemException();
  /**
   * Returns error code.
   * @return windows error code associated with this exception.
   */
  int getErrorCode() const;
  /**
   * Returns system error description.
   * @return system error description.
   */
  const TCHAR *getSystemErrorDescription() const;
private:
  /**
   * Creates formatted message for exception.
   * @param userMessage user description about exception reason.
   * @param errcode windows error code.
   * @fixme document all special cases.
   */
  void createMessage(const TCHAR *userMessage, int errcode);
private:
  /**
   * Description of error from OS.
   */
  StringStorage m_systemMessage;
  /**
   * Windows error code.
   */
  int m_errcode;
};

#endif
