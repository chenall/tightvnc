// Copyright (C) 2012 GlavSoft LLC.
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

#include "WinDxCriticalException.h"
#include "WinDxRecoverableException.h"

// The header including of this cpp file must be at last place to avoid build conflicts.
#include "WinDxgiOutput1.h"

WinDxgiOutput1::WinDxgiOutput1(WinDxgiOutput *dxgiOutput)
: m_dxgiOutput1(0)
{
  HRESULT hr = dxgiOutput->queryInterface(__uuidof(m_dxgiOutput1), reinterpret_cast<void**>(&m_dxgiOutput1));
  if (FAILED(hr)) {
    throw WinDxCriticalException(_T("Can't get IDXGIOutput1"), hr);
  }
}

WinDxgiOutput1::WinDxgiOutput1(const WinDxgiOutput1 &src)
{
  copy(src);
}

WinDxgiOutput1::~WinDxgiOutput1()
{
  if (m_dxgiOutput1 != 0) {
    m_dxgiOutput1->Release();
    m_dxgiOutput1 = 0;
  }
}

WinDxgiOutput1 &WinDxgiOutput1::operator = (WinDxgiOutput1 const &src)
{
  copy(src);
  return *this;
}

void WinDxgiOutput1::copy(const WinDxgiOutput1 &src)
{
  if (this != &src) {
    m_dxgiOutput1 = src.m_dxgiOutput1;
    m_dxgiOutput1->AddRef();
  }
}

IDXGIOutput1 *WinDxgiOutput1::getDxgiOutput1()
{
  return m_dxgiOutput1;
}
