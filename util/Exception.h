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

#ifndef _EXCEPTION_H_
#define _EXCEPTION_H_

#pragma warning(disable:4290)

#include "StringStorage.h"

/**
 * Common Exception class.
 */
class Exception
{
public:
  /**
   * Creates exception with empty description.
   */
  Exception();
  /**
   * Creates exception with specified description.
   * @param format description string in printf-like notation.
   */
  Exception(const TCHAR *format, ...);
  /**
   * Destructor.
   */
  virtual ~Exception();

  /**
   * Returns description of exception.
   */
  const TCHAR *getMessage() const;

protected:
  StringStorage m_message;
};

#endif
