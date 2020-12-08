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

#ifndef __WINDXGIDEVICE_H__
#define __WINDXGIDEVICE_H__

#include "WinD3D11Device.h"

#include <d3d11.h>
#include <DXGI1_2.h>

class WinDxgiDevice
{
public:
  // Uses WinD3D11Device to create internal handle
  WinDxgiDevice(WinD3D11Device *winD3D11Device);
  virtual ~WinDxgiDevice();

  HRESULT getParent(REFIID riid, void **ppvObject);

private:
  IDXGIDevice *m_dxgiDevice;
};

#endif // __WINDXGIDEVICE_H__
