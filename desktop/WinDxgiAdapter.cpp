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
#include "WinDxgiAdapter.h"

WinDxgiAdapter::WinDxgiAdapter(WinDxgiDevice *winDxgiDevice)
: m_dxgiAdapter(0)
{
  HRESULT hr = winDxgiDevice->getParent(__uuidof(IDXGIAdapter), reinterpret_cast<void**>(&m_dxgiAdapter));
  if (FAILED(hr)) {
    throw WinDxCriticalException(_T("Can't GetParent for IDXGIAdapter"), hr);
  }
}

WinDxgiAdapter::~WinDxgiAdapter()
{
  if (m_dxgiAdapter != 0) {
    m_dxgiAdapter->Release();
    m_dxgiAdapter = 0;
  }
}

void WinDxgiAdapter::getDxgiOutput(UINT iOutput, IDXGIOutput **iDxgiOutput)
{
  HRESULT hr = m_dxgiAdapter->EnumOutputs(iOutput, iDxgiOutput);
  if (hr == DXGI_ERROR_NOT_FOUND) {
    StringStorage errMess;
    errMess.format(_T("IDXGIOutput not found for iOutput = %u"), iOutput);
    throw WinDxRecoverableException(errMess.getString(), hr);
  } else if (FAILED(hr)) {
    throw WinDxCriticalException(_T("Can't IDXGIAdapter::EnumOutputs()"), hr);
  }
}
