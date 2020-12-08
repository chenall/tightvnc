// Copyright (C) 2011,2012 GlavSoft LLC.
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

#include "ScrollBar.h"

ScrollBar::ScrollBar() 
: m_hwnd(0), 
  m_vPos(0), m_hPos(0), 
  m_vMax(0), m_hMax(0),
  m_vMin(0), m_hMin(0), 
  m_vStep(0), m_hStep(0),
  m_isVirtualScroll(false),
  m_isVert(false),
  m_isHorz(false),
  m_isVVert(false),
  m_isVHorz(false)
{
}

void ScrollBar::enableVirtualScroll()
{
  if (m_isVert) {
    showVertScroll(false);
    m_isVVert = false;
  }
  if (m_isHorz) {
    showHorzScroll(false);
    m_isVHorz = false;
  }
}

void ScrollBar::disableVirtualScroll()
{
  if (m_isVert != m_isVVert) {
    showVertScroll(m_isVVert);
    if (m_isVVert) {
      setVertRange(m_vMin, m_vMax, m_vStep);
      setVertPos(m_vPos);
    }
  }
  if (m_isHorz != m_isVHorz) {
    showHorzScroll(m_isVHorz);
    if (m_isVHorz) {
      setHorzRange(m_hMin, m_hMax, m_hStep);
      setHorzPos(m_hPos);
    }
  }
}

void ScrollBar::setWindow(HWND hwnd) {
  m_hwnd = hwnd;
}

bool ScrollBar::showVertScroll(bool show) 
{
  if (m_isVirtualScroll) {
    m_isVVert = show;
    return true;
  } else {
    return _showVertScroll(show);
  }
}

bool ScrollBar::_showVertScroll(bool show)
{
  LONG style = GetWindowLong(m_hwnd, GWL_STYLE);
  if (show) {
    style |= WS_VSCROLL;
  } else {
    style &= ~WS_VSCROLL;
  }
  bool result = !!ShowScrollBar(m_hwnd, SB_VERT, show ? TRUE : FALSE);  
  if (result) {
    SetWindowLongPtr(m_hwnd, GWL_STYLE, style);
    m_isVert = show;
  }
  return result;
}

bool ScrollBar::showHorzScroll(bool show)
{
  if (m_isVirtualScroll) {
    m_isVHorz = show;
    return true;
  } else {
    return _showHorzScroll(show);
  }
}

bool ScrollBar::_showHorzScroll(bool show) 
{
  LONG style = GetWindowLong(m_hwnd, GWL_STYLE);
  if (show) {
    style |=  WS_HSCROLL;
  } else {
    style &= ~WS_HSCROLL;
  }
  bool result = !!ShowScrollBar(m_hwnd, SB_HORZ, show ? TRUE : FALSE);  
  if (result) {
    SetWindowLong(m_hwnd, GWL_STYLE, style);
    m_isHorz = show;
  }
  return result;
}

void ScrollBar::setHorzRange(int imin, int imax, int istep) 
{
  m_hMin  = imin;
  m_hMax  = imax;
  m_hStep = istep;

  if (!m_isVirtualScroll) {
    SCROLLINFO si;

    ZeroMemory(&si, sizeof(SCROLLINFO));
    si.cbSize = sizeof(SCROLLINFO);
    si.nMin   = imin;
    si.nMax   = imax;
    si.fMask  = SIF_RANGE | SIF_PAGE;
    si.nPage  = istep;
    SetScrollInfo(m_hwnd, SB_HORZ, &si, TRUE);
  }
}

void ScrollBar::setVertRange(int imin, int imax, int istep) {
  m_vMin  = imin;
  m_vMax  = imax;
  m_vStep = istep;

  if (!m_isVirtualScroll) {
    SCROLLINFO si;

    ZeroMemory(&si, sizeof(SCROLLINFO));
    si.cbSize = sizeof(SCROLLINFO);
    si.nMin   = imin;
    si.nMax   = imax;
    si.fMask  = SIF_RANGE | SIF_PAGE;
    si.nPage  = istep;
    SetScrollInfo(m_hwnd, SB_VERT, &si, TRUE);
  }
}

void ScrollBar::setVertPos(int iPos) {
  if (!m_isVirtualScroll) {
    SCROLLINFO si;

    ZeroMemory(&si, sizeof(SCROLLINFO));
    si.cbSize = sizeof(SCROLLINFO);
    si.fMask  = SIF_POS;
    si.nPos   = iPos;
    m_vPos    = SetScrollInfo(m_hwnd, SB_VERT, &si, TRUE);
  } else {
    m_vPos    = iPos;
  }
}

void ScrollBar::setHorzPos(int iPos) {
  if (!m_isVirtualScroll) {
    SCROLLINFO si;

    ZeroMemory(&si, sizeof(SCROLLINFO));
    si.cbSize = sizeof(SCROLLINFO);
    si.fMask  = SIF_POS;
    si.nPos   = iPos;
    m_hPos    = SetScrollInfo(m_hwnd, SB_HORZ, &si, TRUE);
  } else {
    m_hPos    = iPos;
  }
}

void ScrollBar::moveUpVert(int iPercent) {
  int iShift = m_vStep; 

  if (iPercent) {
    iShift = iShift * iPercent / 100;
    if (!iShift) {
      iShift++;
    }
  }

  m_vPos += iShift;
  if (m_vPos > m_vMax) {
    m_vPos = m_vMax;
  }
  return setVertPos(m_vPos);
}

void ScrollBar::moveDownVert(int iPercent) {
  int iShift = m_vStep;

  if (iPercent) {
    iShift = iShift * iPercent / 100;
    if (!iShift) {
      iShift++;
    }
  }

  m_vPos -= iShift;
  if (m_vPos < m_vMin) {
    m_vPos = m_vMin;
  }
  return setVertPos(m_vPos);
}

void ScrollBar::moveLeftHorz(int iPercent) {
  int iShift = m_hStep;

  if (iPercent) {
    iShift = iShift * iPercent / 100;
    if (!iShift) {
      iShift++;
    }
  }

  m_hPos -= iShift;
  if (m_hPos < m_hMin) {
    m_hPos = m_hMin;
  }
  return setHorzPos(m_hPos);
}

void ScrollBar::moveRightHorz(int iPercent) {
  int iShift = m_vStep;

  if (iPercent) {
    iShift = iShift * iPercent / 100;
    if (!iShift) {
      iShift++;
    }
  }

  m_hPos += iShift;
  if (m_hPos > m_hMax) {
    m_hPos = m_hMax;
  }
  return setHorzPos(m_hPos);
}

int ScrollBar::getVertPos() {
  return m_vPos;
}

int ScrollBar::getHorzPos() {
  return m_hPos;
}

int ScrollBar::getVerticalSize()
{
  return GetSystemMetrics(SM_CXVSCROLL);
}

int ScrollBar::getHorizontalSize()
{
  return GetSystemMetrics(SM_CXHSCROLL);
}
