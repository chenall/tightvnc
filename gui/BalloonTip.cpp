// Copyright (C) 2009,2010,2011,2012 GlavSoft LLC.
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

#include "BalloonTip.h"

BalloonTip::BalloonTip(const TCHAR *text, const TCHAR *caption)
{
  setText(text);
  setTitle(caption);
}

BalloonTip::BalloonTip()
{
  setText(_T(""));
  setTitle(_T(""));
}

BalloonTip::~BalloonTip()
{
}

void BalloonTip::showTooltip(Control *control)
{
  MessageBox(control->getWindow(), m_text.getString(), m_title.getString(), MB_OK | MB_ICONWARNING);
}

void BalloonTip::setText(const TCHAR *text)
{
  m_text.setString(text);
}

void BalloonTip::setTitle(const TCHAR *title)
{
  m_title.setString(title);
}

void BalloonTip::getText(StringStorage *text) const
{
  text->setString(m_text.getString());
}

void BalloonTip::getTitle(StringStorage *title) const
{
  title->setString(m_title.getString());
}

void BalloonTip::setIconType(int iconType)
{
}

int BalloonTip::getIconType() const
{
  return 0;
}
