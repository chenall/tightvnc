// Copyright (C) 2008,2009,2010,2011,2012 GlavSoft LLC.
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

#include "PortMapping.h"
#include "util/StringParser.h"

#include <tchar.h>
#include <stdio.h>

PortMapping::PortMapping()
: m_port(0)
{
}

PortMapping::PortMapping(int nport, PortMappingRect nrect)
: m_port(nport), m_rect(nrect)
{
}

PortMapping::PortMapping(const PortMapping &other)
: m_port(other.m_port), m_rect(other.m_rect)
{
}

PortMapping::~PortMapping()
{
}

PortMapping &PortMapping::operator=(const PortMapping &other)
{
  m_port = other.m_port;
  m_rect = other.m_rect;
  return *this;
}

bool PortMapping::isEqualTo(const PortMapping *other) const
{
  return other->m_port == m_port && other->m_rect.isEqualTo(&m_rect);
}

void PortMapping::setPort(int nport)
{
  m_port = nport;
}

void PortMapping::setRect(PortMappingRect nrect)
{
  m_rect = nrect;
}

int PortMapping::getPort() const
{
  return m_port;
}

PortMappingRect PortMapping::getRect() const
{
  return m_rect;
}

void PortMapping::toString(StringStorage *string) const
{
  //
  // Format:   [port]:[rect.toString()]
  // It means: [port]:[width]x[height]+[x]+[y]
  // without square brackets.
  //

  StringStorage rectString;
  m_rect.toString(&rectString);

  string->format(_T("%d:%s"), m_port, rectString.getString());
}

bool PortMapping::parse(const TCHAR *str, PortMapping *mapping)
{
  int port;
  TCHAR c;
  PortMappingRect rect;
  const TCHAR *rectString = _tcschr(str, _T(':')) + 1;
  if (rectString == NULL) {
    return false;
  }
  if ((_stscanf(str, _T("%d%c"), &port, &c) != 2) || (c != _T(':'))) {
    return false;
  }
  if (port < 0) {
    return false;
  }
  if (!PortMappingRect::parse(rectString, &rect)) {
    return false;
  }
  if (mapping != NULL) {
    mapping->setPort(port);
    mapping->setRect(rect);
  }
  return true;
}
