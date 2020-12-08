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

#ifndef __WIND3D11TEXTURE2D_H__
#define __WIND3D11TEXTURE2D_H__

#include <d3d11.h>
#include <DXGI1_2.h>

class WinD3D11Texture2D
{
public:
  WinD3D11Texture2D(ID3D11Device *device, const D3D11_TEXTURE2D_DESC *deskTexture);
  // Texture will be created by the IDXGIResource->QueryInterface() function call.
  WinD3D11Texture2D(IDXGIResource *dxgiResource);

  WinD3D11Texture2D(const WinD3D11Texture2D & other);

  virtual ~WinD3D11Texture2D();

  ID3D11Texture2D *getTexture() const;

private:
  ID3D11Texture2D *m_dxgiTexture;
};

#endif // __WIND3D11TEXTURE2D_H__
