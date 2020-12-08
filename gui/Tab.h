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

#ifndef _TAB_H_
#define _TAB_H_

#include "BaseDialog.h"
#include "Control.h"
#include "util/StringStorage.h"

class Tab
{
public:
  Tab();
  Tab(BaseDialog *dialog, const TCHAR *caption);

  //
  // Access methods to protected members
  //

  void setCaption(const TCHAR *caption) { m_caption.setString(caption); }

  const TCHAR *getCaption() {
    return m_caption.getString();
  }

  void setDialog(BaseDialog *dialog) { m_dialog = dialog; }
  BaseDialog *getDialog() { return m_dialog; }

  //
  // Method return true if tab has dialog
  //

  bool isOk() { return m_dialog != NULL; }

  //
  // Changes visible state of dialog donates by this tab
  //

  void setVisible(bool visible);

protected:

  BaseDialog *m_dialog;
  StringStorage m_caption;
};

#endif
