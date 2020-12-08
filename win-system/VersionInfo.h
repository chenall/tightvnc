// Copyright (C) 2010,2011,2012 GlavSoft LLC.
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

#ifndef _VERSION_INFO_H_
#define _VERSION_INFO_H_

#include "util/CommonHeader.h"
#include "SystemException.h"

/**
 * Contains information about executable file version info.
 */
class VersionInfo
{
public:
  /**
   * Creates version info of specified file.
   * @param pathToFile path to executable file to get version info from.
   * @throws SystemException on fail.
   */
  VersionInfo(const TCHAR *pathToFile) throw(SystemException);

  /**
   * Returns product version info string.
   * @return product version info string.
   */
  const TCHAR *getProductVersionString() const;

private:
  /**
   * Product version as string.
   */
  StringStorage m_productVersionString;
};

#endif
