// Copyright (C) 2010,2011,2012 GlavSoft LLC.
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

#include "DispatchCommand.h"

DispatchCommand::DispatchCommand(ControlProxy *serverControl,
                                 const TCHAR *dispatchSpec)
: m_proxy(serverControl),
  m_dispatchSpec(dispatchSpec)
{
}

DispatchCommand::~DispatchCommand()
{
}

void DispatchCommand::execute()
{
  // Parse dispatcher specification
  StringStorage name;
  UINT32 id;

  size_t slashPos = m_dispatchSpec.findChar(_T('/'));
  if (slashPos == (size_t)-1) {
    name = m_dispatchSpec;
    id = 0;
  } else {
    m_dispatchSpec.getSubstring(&name, 0, slashPos - 1);
    const TCHAR *str = m_dispatchSpec.getString();
    id = (UINT32)_tstol(&str[slashPos + 1]);
  }

  m_proxy->makeTcpDispatcherConnection(name.getString(), _T(""), _T(""), id);
}
