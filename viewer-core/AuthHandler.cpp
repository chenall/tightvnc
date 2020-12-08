// Copyright (C) 2011,2012 GlavSoft LLC.
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

#include "AuthHandler.h"

AuthException::AuthException(const TCHAR *message)
: Exception(message),
  m_authErrorCode(AUTH_ERROR)
{
}

AuthException::~AuthException()
{
}

int AuthException::getAuthCode() const
{
  return m_authErrorCode;
}

AuthUnknownException::AuthUnknownException(const TCHAR *message)
: AuthException(message)
{
  m_authErrorCode = AUTH_UNKNOWN_TYPE;
}

AuthUnknownException::~AuthUnknownException()
{
}

AuthCanceledException::AuthCanceledException(const TCHAR *message)
: AuthException(message)
{
  m_authErrorCode = AUTH_CANCELED;
}

AuthCanceledException::~AuthCanceledException()
{
}

AuthHandler::AuthHandler(int authType)
: m_id(authType)
{
}

AuthHandler::~AuthHandler()
{
}

int AuthHandler::getType() const
{
  return m_id;
}
