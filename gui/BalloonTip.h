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

#ifndef _BALLOON_TIP_H_
#define _BALLOON_TIP_H_

#include "Tooltip.h"
#include "util/CommonHeader.h"
#include <commctrl.h>

/**
 * @deprecated, use ToolTip instead.
 */
class BalloonTip : public Tooltip
{
public:
  BalloonTip(const TCHAR *text, const TCHAR *caption);
  BalloonTip();
  virtual ~BalloonTip();

  void showTooltip(Control *control);

  void setText(const TCHAR *text);
  void setTitle(const TCHAR *caption);

  void getText(StringStorage *text) const;
  void getTitle(StringStorage *title) const;

  void setIconType(int iconType);
  int getIconType() const;

protected:
  StringStorage m_text;
  StringStorage m_title;
};

#endif
