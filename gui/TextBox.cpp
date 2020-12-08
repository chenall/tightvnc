// Copyright (C) 2008,2009,2010,2011,2012 GlavSoft LLC.
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

#include "TextBox.h"

TextBox::~TextBox()
{
}

void TextBox::setCaretPos(int h, int v)
{
  SendMessage(m_hwnd, EM_LINESCROLL, h, v);
}

size_t TextBox::getTextLengthLimit()
{
  return SendMessage(m_hwnd, EM_GETLIMITTEXT, 0, 0);
}

void TextBox::setTextLengthLimit(size_t n)
{
  SendMessage(m_hwnd, EM_SETLIMITTEXT, n, 0);
}

int TextBox::getCurrentLineIndex()
{
  return (int)SendMessage(m_hwnd, EM_LINEINDEX, -1, 0);
}

int TextBox::getLineCount()
{
  return (int)SendMessage(m_hwnd, EM_GETLINECOUNT, 0, 0);
}

//
// FIXME: Unimplemented
//

int TextBox::getCaretPos()
{
  return 0;
}

void TextBox::selectText(size_t startPos, size_t endPos)
{
  SendMessage(getWindow(), EM_SETSEL, startPos, endPos);
}

void TextBox::showBalloonTip(Tooltip *tip)
{
  tip->showTooltip(this);
}

