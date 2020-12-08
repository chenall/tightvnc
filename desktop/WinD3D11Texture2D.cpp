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

#include "WinDxRecoverableException.h"

// The header including of this cpp file must be at last place to avoid build conflicts.
#include "WinD3D11Texture2D.h"

WinD3D11Texture2D::WinD3D11Texture2D(ID3D11Device *device, const D3D11_TEXTURE2D_DESC *deskTexture)
: m_dxgiTexture(0)
{
  HRESULT hr = device->CreateTexture2D(deskTexture, 0, &m_dxgiTexture);
  if (FAILED(hr) || m_dxgiTexture == 0) {
    throw WinDxRecoverableException(_T("Can't CreateTexture2D()"), hr);
  }
}

WinD3D11Texture2D::WinD3D11Texture2D(IDXGIResource *dxgiResource)
: m_dxgiTexture(0)
{
  HRESULT hr = dxgiResource->QueryInterface(__uuidof(ID3D11Texture2D), reinterpret_cast<void **>(&m_dxgiTexture));
  if (FAILED(hr) || m_dxgiTexture == 0) {
    throw WinDxRecoverableException(_T("Can't QueryInterface() to create ID3D11Texture2D"), hr);
  }
}

WinD3D11Texture2D::WinD3D11Texture2D(const WinD3D11Texture2D & other)
{ 
  m_dxgiTexture = other.getTexture(); 
  m_dxgiTexture->AddRef(); 
};


WinD3D11Texture2D::~WinD3D11Texture2D()
{
  if (m_dxgiTexture != 0) {
    m_dxgiTexture->Release();
  }
}

ID3D11Texture2D *WinD3D11Texture2D::getTexture() const
{
  return m_dxgiTexture;
}
