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

#ifndef __MSIPROPERTIES_H__
#define __MSIPROPERTIES_H__

#include "util/CommonHeader.h"
#include <strsafe.h>
#include <msiquery.h>

class MsiProperties
{
public:
  MsiProperties(MSIHANDLE handle);
  ~MsiProperties();

  int getInt32(const TCHAR *name);
  void setInt32(const TCHAR *name, int value);

  void getString(const TCHAR *name, StringStorage *out);
  void setString(const TCHAR *name, const StringStorage *value);

private:
  void getValue(const TCHAR *name, StringStorage *out);
  void setValue(const TCHAR *name, const StringStorage *value);

  MSIHANDLE m_handle;
};

#endif // __MSIPROPERTIES_H__
