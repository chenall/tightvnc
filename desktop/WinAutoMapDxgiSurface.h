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

#ifndef __WINAUTOMAPDXGISURFACE_H__
#define __WINAUTOMAPDXGISURFACE_H__

#include "region/Rect.h"

#include <d3d11.h>
#include <DXGI1_2.h>

#include "WinDxgiSurface.h"

// Calls IDXGISurface->Map() function at the constructor and Unmap() at the destructor.
class WinAutoMapDxgiSurface
{
public:
  // Surface will be created by the ID3D11Texture2D's QueryInterface() function call.
  WinAutoMapDxgiSurface(WinDxgiSurface *surface, UINT mapFlags);
  virtual ~WinAutoMapDxgiSurface();

  size_t getStride() const;
  char *getBuffer() const;

private:
  WinDxgiSurface *m_surface;
  DXGI_MAPPED_RECT m_mappedRect;
};

#endif // __WINAUTOMAPDXGISURFACE_H__
