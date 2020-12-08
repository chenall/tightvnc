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

#include "AppletParameter.h"

#include "util/CommonHeader.h"

#include <crtdbg.h>

AppletParameter::AppletParameter(const char *name, const char *value)
: m_isValid(false)
{
  _ASSERT(name != NULL);
  _ASSERT(value != NULL);

  m_isValid = isStringValid(name) && isStringValid(value);

  if (isValid()) {
    char format[] = "<PARAM NAME=\"%s\" VALUE=\"%s\" >\n";
    m_formattedString.format(format, name, value);
  }
}

AppletParameter::~AppletParameter()
{
}

const char *AppletParameter::getFormattedString() const
{
  return m_formattedString.getString();
}

bool AppletParameter::isValid() const
{
  return m_isValid;
}

bool AppletParameter::isStringValid(const char *str) const
{
  // Applet argument can contain alnum, '_', '.', ' ' characters.
  for (size_t i = 0; i < strlen(str); i++) {
    if (!(isalnum(str[i]) || str[i] == '_' || str[i] == '.' || str[i] == ' ')) {
      return false;
    } // if character is not valid.
  } // for all characters if value.

  return true;
}
