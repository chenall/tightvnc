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

#ifndef __WINDXGIOUTPUTDUPLICATION_H__
#define __WINDXGIOUTPUTDUPLICATION_H__

#include "WinDxgiOutput1.h"
#include "rfb/CursorShape.h"

#include <d3d11.h>
#include <DXGI1_2.h>

// This class is a wrapper for the IDXGIOutputDuplication interface.
class WinDxgiOutputDuplication
{
public:
  WinDxgiOutputDuplication(WinDxgiOutput1 *dxgiOutput, WinD3D11Device *d3D11Device);
  WinDxgiOutputDuplication(const WinDxgiOutputDuplication &src);
  virtual ~WinDxgiOutputDuplication();

  WinDxgiOutputDuplication &operator = (WinDxgiOutputDuplication const &src);

  // Return pointer to a IDXGIOutputDuplication object. The pointer will be valid until
  // this object destructor has been called.
  IDXGIOutputDuplication *getDxgiOutputDuplication();

  // Throws WinDxException on an error.
  // Returns count of got "move" rects.
  // Also, the function resize the moveRects vector if it's needed.
  size_t getFrameMoveRects(std::vector<DXGI_OUTDUPL_MOVE_RECT> *moveRects);

  // Throws WinDxException on an error.
  // Returns count of got "dirty" rects.
  // Also, the function resize the dirtyRects vector if it's needed.
  size_t getFrameDirtyRects(std::vector<RECT> *dirtyRects);

  // Throws WinDxException on an error.
  // Modifies cursorShape with new data.
  void getFrameCursorShape(CursorShape *cursorShape, UINT pointerShapeBufferSize);

private:
  void copy(const WinDxgiOutputDuplication &src);

  IDXGIOutputDuplication *m_outDupl;
};

#endif // __WINDXGIOUTPUTDUPLICATION_H__
