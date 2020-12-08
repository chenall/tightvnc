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

#ifndef __WINDXGIOUTPUT_H__
#define __WINDXGIOUTPUT_H__

#include "region/Rect.h"

#include "WinDxgiAdapter.h"

#include <d3d11.h>
#include <DXGI1_2.h>

class WinDxgiOutput
{
public:
  WinDxgiOutput(WinDxgiAdapter *dxgiAdapter, UINT iOutput);
  WinDxgiOutput(const WinDxgiOutput &src);
  virtual ~WinDxgiOutput();

  WinDxgiOutput &operator = (WinDxgiOutput const &src);

  bool isAttachedtoDesktop();

  Rect getDesktopCoordinates();

  DXGI_MODE_ROTATION getRotation() const;

  void getDeviceName(StringStorage *out);

  HRESULT queryInterface(REFIID riid, void **ppvObject);

private:
  void copy(const WinDxgiOutput &src);

  IDXGIOutput *m_dxgiOutput;
  DXGI_OUTPUT_DESC m_description;
};

#endif // __WINDXGIOUTPUT_H__
