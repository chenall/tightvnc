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

#include "util/Exception.h"

// The header including of this cpp file must be at last place to avoid build conflicts.
#include "WinDxgiDevice.h"

WinDxgiDevice::WinDxgiDevice(WinD3D11Device *winD3D11Device)
: m_dxgiDevice(0)
{
  HRESULT hr = winD3D11Device->deviceQueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void**>(&m_dxgiDevice));
  if (FAILED(hr)) {
    StringStorage errMess;
    errMess.format(_T("Can't QueryInterface for IDXGIDevice (%l)"), (long)hr);
    throw Exception(errMess.getString());
  }
}

WinDxgiDevice::~WinDxgiDevice()
{
  if (m_dxgiDevice != 0) {
    m_dxgiDevice->Release();
    m_dxgiDevice = 0;
  }
}

HRESULT WinDxgiDevice::getParent(REFIID riid, void **ppvObject)
{
  return m_dxgiDevice->GetParent(riid, ppvObject);
}
