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

#ifndef __WIND3D11DEVICE_H__
#define __WIND3D11DEVICE_H__

#include "region/Rect.h"
#include "win-system/DynamicLibrary.h"
#include "log-writer/LogWriter.h"

#include <d3d11.h>
#include <DXGI1_2.h>

class WinD3D11Device
{
public:
  // Creates new device and context of first found.
  WinD3D11Device();
  WinD3D11Device(LogWriter *log);
  // Copy references and increase count for winD3D11Device's internal handles. So the
  // source winD3D11Device object can be destroyed while this object will use.
  WinD3D11Device(const WinD3D11Device &src);
  virtual ~WinD3D11Device();

  WinD3D11Device &operator = (WinD3D11Device const &src);

  HRESULT deviceQueryInterface(REFIID riid, void **ppvObject);
  HRESULT contextQueryInterface(REFIID riid, void **ppvObject);

  ID3D11Device *getDevice();
  ID3D11DeviceContext *getContext();

  // A wrap for the ID3D11DeviceContext::CopySubresourceRegion() function.
  void copySubresourceRegion(ID3D11Texture2D *dstTexture2D, int dstX, int dstY,
                             ID3D11Texture2D *srcTexture2D, const Rect *srcRect,
                             UINT front, UINT back);
private:
  void copy(const WinD3D11Device &src);

  DynamicLibrary m_d3d11Lib;
  ID3D11Device *m_device;
  ID3D11DeviceContext *m_context;

  LogWriter *m_log;
};

#endif // __WIND3D11DEVICE_H__
