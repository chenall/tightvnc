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

#ifndef __WINDXGIOUTPUT1_H__
#define __WINDXGIOUTPUT1_H__

#include "region/Rect.h"

#include "WinDxgiOutput.h"

#include <d3d11.h>
#include <DXGI1_2.h>

class WinDxgiOutput1
{
public:
  WinDxgiOutput1(WinDxgiOutput *dxgiOutput);
  WinDxgiOutput1(const WinDxgiOutput1 &src);
  virtual ~WinDxgiOutput1();

  WinDxgiOutput1 &operator = (WinDxgiOutput1 const &src);

  // Return pointer to a IDXGIOutput1 object. The pointer will be valid until
  // this object destructor has been called.
  IDXGIOutput1 *getDxgiOutput1();

private:
  void copy(const WinDxgiOutput1 &src);

  IDXGIOutput1 *m_dxgiOutput1;
};

#endif // __WINDXGIOUTPUT1_H__
