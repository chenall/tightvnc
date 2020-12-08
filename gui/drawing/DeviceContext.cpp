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

#include "DeviceContext.h"

DeviceContext::DeviceContext(HWND window)
: m_hasOwnDC(false), m_wnd(window)
{
  m_dc = GetDC(window);
}

DeviceContext::DeviceContext(DeviceContext* compatibleDevice)
: m_hasOwnDC(true)
{
  m_wnd = compatibleDevice->m_wnd;
  m_dc = CreateCompatibleDC(compatibleDevice->m_dc);
}

DeviceContext::DeviceContext(PaintWindow* pntWnd)
: m_wnd(0), m_hasOwnDC(false)
{
  m_dc = pntWnd->getHDCPaint();
}

DeviceContext::~DeviceContext()
{
  if (m_wnd) {
    ReleaseDC(m_wnd, m_dc);
  }
  if (m_hasOwnDC) {
    DeleteDC(m_dc);
  }
}

HGDIOBJ DeviceContext::selectObject(HGDIOBJ object)
{
  return SelectObject(m_dc, object);
}
