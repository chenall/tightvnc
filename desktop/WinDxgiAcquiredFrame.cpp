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

#include "WinDxRecoverableException.h"

// The header including of this cpp file must be at last place to avoid build conflicts.
#include "WinDxgiAcquiredFrame.h"

WinDxgiAcquiredFrame::WinDxgiAcquiredFrame(WinDxgiOutputDuplication *outDupl, UINT timeOutMilliSec)
: m_wasTimeOut(false),
  m_desktopResource(0),
  m_outDupl(*outDupl)
{
  ZeroMemory(&m_frameInfo, sizeof(m_frameInfo));
  HRESULT hr = m_outDupl.getDxgiOutputDuplication()->AcquireNextFrame(timeOutMilliSec, &m_frameInfo, &m_desktopResource);
  if (hr == DXGI_ERROR_WAIT_TIMEOUT) {
    m_wasTimeOut = true;
  } else if (FAILED(hr)) {
    throw WinDxRecoverableException(_T("Can't AcquireNextFrame()"), hr);
  }
}

WinDxgiAcquiredFrame::~WinDxgiAcquiredFrame()
{
  if (m_desktopResource != 0) {
    m_desktopResource->Release();
    m_desktopResource = 0;
  }
  m_outDupl.getDxgiOutputDuplication()->ReleaseFrame();
}

bool WinDxgiAcquiredFrame::wasTimeOut()
{
  return m_wasTimeOut;
}

IDXGIResource *WinDxgiAcquiredFrame::getDxgiResource()
{
  return m_desktopResource;
}

DXGI_OUTDUPL_FRAME_INFO *WinDxgiAcquiredFrame::getFrameInfo()
{
  return &m_frameInfo;
}
