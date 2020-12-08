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

#ifndef __WINDXGISURFACE_H__
#define __WINDXGISURFACE_H__

#include <d3d11.h>
#include <DXGI1_2.h>

class WinDxgiSurface
{
public:
  // Surface will be created by the ID3D11Texture2D's QueryInterface() function call.
  WinDxgiSurface(ID3D11Texture2D *texture2D);
  virtual ~WinDxgiSurface();

  IDXGISurface *getSurface() const;

private:
  IDXGISurface *m_dxgiSurface;
};

#endif // __WINDXGISURFACE_H__
