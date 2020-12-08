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
#include "region/Rect.h"

// The header including of this cpp file must be at last place to avoid build conflicts.
#include "WinD3D11Device.h"

typedef HRESULT (WINAPI *D3D11CreateDeviceFunType)(
  _In_opt_ IDXGIAdapter* pAdapter,
  D3D_DRIVER_TYPE DriverType,
  HMODULE Software,
  UINT Flags,
  _In_reads_opt_( FeatureLevels ) CONST D3D_FEATURE_LEVEL* pFeatureLevels,
  UINT FeatureLevels,
  UINT SDKVersion,
  _Out_opt_ ID3D11Device** ppDevice,
  _Out_opt_ D3D_FEATURE_LEVEL* pFeatureLevel,
  _Out_opt_ ID3D11DeviceContext** ppImmediateContext );

WinD3D11Device::WinD3D11Device(LogWriter *log)
: m_device(0),
  m_context(0),
  m_d3d11Lib(_T("d3d11.dll")),
  m_log(log)
{
  D3D11CreateDeviceFunType d3d11CreateDevice;
  d3d11CreateDevice = (D3D11CreateDeviceFunType)m_d3d11Lib.getProcAddress("D3D11CreateDevice");
  if (d3d11CreateDevice == 0) {
    throw Exception(_T("Unable to load the D3D11CreateDevice() function"));
  }

  // Driver types supported
  D3D_DRIVER_TYPE driverTypes[] =
  {
    D3D_DRIVER_TYPE_HARDWARE
  };
  UINT driverTypeCount = ARRAYSIZE(driverTypes);

  // Feature levels supported
  D3D_FEATURE_LEVEL featureLevels[] =
  {
    D3D_FEATURE_LEVEL_11_0,
    D3D_FEATURE_LEVEL_10_1,
    D3D_FEATURE_LEVEL_10_0,
    D3D_FEATURE_LEVEL_9_1
  };
  UINT featureLevelCount = ARRAYSIZE(featureLevels);

  D3D_FEATURE_LEVEL featureLevel;

  // Create device
  HRESULT hr;
  for (UINT iDriverType = 0; iDriverType < driverTypeCount; ++iDriverType) {
    m_log->debug(_T("Creating of (%u) driverType device"), iDriverType);
    hr = d3d11CreateDevice(0,
                           driverTypes[iDriverType],
                           0,
                           0,
                           featureLevels,
                           featureLevelCount,
                           D3D11_SDK_VERSION,
                           &m_device,
                           &featureLevel,
                           &m_context);
    if (SUCCEEDED(hr)) {
      m_log->debug(_T("Creating of %u driverType device is successfull, supported D3D_FEATURE_LEVEL is %u"), iDriverType, featureLevel);
      break;
    }
  }
  if (FAILED(hr)) {
    StringStorage errMess;
    errMess.format(_T("D3D11CreateDevice function was failed with code error = (%dl)"), (long)hr);
    m_log->debug(_T("D3D11CreateDevice function was failed with code error = (%dl)"), (long)hr);
    throw Exception(errMess.getString());
  }
}

WinD3D11Device::WinD3D11Device(const WinD3D11Device &src)
{
  copy(src);
}

WinD3D11Device::~WinD3D11Device()
{
  m_log->debug(_T("Release ID3D11Device"));
  if (m_device != 0) {
    m_device->Release();
    m_device = 0;
  }
  m_log->debug(_T("Release ID3D11DeviceContext"));
  if (m_context != 0) {
    m_context->Release();
    m_context = 0;
  }
}

WinD3D11Device &WinD3D11Device::operator = (WinD3D11Device const &src)
{
  copy(src);
  return *this;
}

void WinD3D11Device::copy(const WinD3D11Device &src)
{
  if (this != &src) {
    m_device = src.m_device;
    m_device->AddRef();
    m_context = src.m_context;
    m_context->AddRef();
  }
}

HRESULT WinD3D11Device::deviceQueryInterface(REFIID riid, void **ppvObject)
{
  return m_device->QueryInterface(riid, ppvObject);
}

HRESULT WinD3D11Device::contextQueryInterface(REFIID riid, void **ppvObject)
{
  return m_context->QueryInterface(riid, ppvObject);
}

ID3D11Device *WinD3D11Device::getDevice()
{
  return m_device;
}

ID3D11DeviceContext *WinD3D11Device::getContext()
{
  return m_context;
}

void WinD3D11Device::copySubresourceRegion(ID3D11Texture2D *dstTexture2D, int dstX, int dstY,
                                           ID3D11Texture2D *srcTexture2D, const Rect *srcRect,
                                           UINT front, UINT back)
{
  D3D11_BOX box;
  box.left = srcRect->left;
  box.top = srcRect->top;
  box.right = srcRect->right;
  box.bottom = srcRect->bottom;
  box.front = front;
  box.back = back;
  m_context->CopySubresourceRegion(dstTexture2D, 0, dstX, dstY, 0, srcTexture2D, 0, &box);
}
