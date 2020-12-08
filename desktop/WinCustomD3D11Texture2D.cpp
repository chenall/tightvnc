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

// The header including of this cpp file must be at last place to avoid build conflicts.
#include "WinCustomD3D11Texture2D.h"

WinCustomD3D11Texture2D::Texture2DDescInitializer::Texture2DDescInitializer(UINT width, UINT height,
                                                                            DXGI_MODE_ROTATION rotation)
{
  if (rotation == DXGI_MODE_ROTATION_ROTATE90 || rotation == DXGI_MODE_ROTATION_ROTATE270) {
    m_desc.Width = height;
    m_desc.Height = width;
  } else {
    m_desc.Width = width;
    m_desc.Height = height;
  }
  m_desc.MipLevels = 1;
  m_desc.ArraySize = 1;
  m_desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
  m_desc.SampleDesc.Count = 1;
  m_desc.SampleDesc.Quality = 0;
  m_desc.Usage = D3D11_USAGE_STAGING;
  m_desc.BindFlags = 0;
  m_desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
  m_desc.MiscFlags = 0;
}

const D3D11_TEXTURE2D_DESC *WinCustomD3D11Texture2D::Texture2DDescInitializer::getDesc() const
{
  return &m_desc;
}

WinCustomD3D11Texture2D::WinCustomD3D11Texture2D(ID3D11Device *device, UINT width, UINT height,
                                                 DXGI_MODE_ROTATION rotation)
: m_textDescInitializer(width, height, rotation),
  m_textureWrapper(device, m_textDescInitializer.getDesc())
{
}

WinCustomD3D11Texture2D::WinCustomD3D11Texture2D(const WinCustomD3D11Texture2D &other)
  : m_textDescInitializer(other.m_textDescInitializer),
    m_textureWrapper(other.m_textureWrapper)
{
}

void WinCustomD3D11Texture2D::operator= (const WinCustomD3D11Texture2D &other) 
{
  m_textDescInitializer = other.m_textDescInitializer;
  m_textureWrapper = other.m_textureWrapper;
}

WinCustomD3D11Texture2D::~WinCustomD3D11Texture2D()
{

}

ID3D11Texture2D *WinCustomD3D11Texture2D::getTexture() const
{
  return m_textureWrapper.getTexture();
}

const D3D11_TEXTURE2D_DESC *WinCustomD3D11Texture2D::getDesc() const
{
  return m_textDescInitializer.getDesc();
}
