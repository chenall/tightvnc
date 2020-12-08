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

#include "SystemException.h"

#include <crtdbg.h>

SystemException::SystemException()
: Exception(), m_errcode(GetLastError())
{
  createMessage(0, m_errcode);
}

SystemException::SystemException(int errcode)
: Exception(), m_errcode(errcode)
{
  createMessage(0, m_errcode);
}

SystemException::SystemException(const TCHAR *userMessage)
: Exception(), m_errcode(GetLastError())
{
  createMessage(userMessage, m_errcode);
}

SystemException::SystemException(const TCHAR *userMessage, int errcode)
: Exception(), m_errcode(errcode)
{
  createMessage(userMessage, m_errcode);
}

SystemException::~SystemException()
{
}

int SystemException::getErrorCode() const
{
  return m_errcode;
}

const TCHAR *SystemException::getSystemErrorDescription() const
{
  return m_systemMessage.getString();
}

void SystemException::createMessage(const TCHAR *userMessage, int errcode)
{
  if (userMessage == 0 && errcode == ERROR_SUCCESS) {
    userMessage = _T("Thrown a system exception but the program")
                  _T(" cannot identify the corresponding system error.");
  }

  // Get description of windows specific error.

  bool formatMessageOk = true;

  TCHAR buffer[1024 * 10] = {0};

  if (FormatMessage(
    FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
    NULL,
    errcode,
    MAKELANGID(LANG_ENGLISH, SUBLANG_DEFAULT),
    (LPTSTR)&buffer[0],
    sizeof(buffer),
    NULL) == 0) {
     formatMessageOk = false;
  }

  StringStorage windowsErrorDescription(buffer);

  // Remove bad characters.

  const TCHAR badCharacters[] = { 10, 13, _T('\n'), _T('\t') };

  windowsErrorDescription.removeChars(badCharacters, sizeof(badCharacters) / sizeof(TCHAR));

  if (windowsErrorDescription.endsWith(_T('.'))) {
    windowsErrorDescription.truncate(1);
  }

  // Create system error part of message.

  if (formatMessageOk) {
    m_systemMessage.format(_T("%s (error code %d)"),
      windowsErrorDescription.getString(),
      errcode);
  } else {
    m_systemMessage.format(_T("Error code %d"), errcode);
  }

  // Use user message if specified.

  if (errcode != 0) {
    if (userMessage == 0) {
      m_message = m_systemMessage;
    } else {
      m_message.format(_T("%s (system error: %s)"),
        userMessage,
        m_systemMessage.getString());
    }
  } else {
    m_message = userMessage;
    m_systemMessage = userMessage;
  }
}
