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

#ifndef _TVNLOGFILENAME_H_
#define _TVNLOGFILENAME_H_

#include "util/CommonHeader.h"

/**
 * Utility to create path to server log file.
 */
class TvnLogFilename
{
public:
  /**
   * Queries full path to directory where server log files are located.
   *
   * @param forService determinates if it must create log directory path for service, or for application.
   * @param shareToAll determinates if log directory must be shared to all users.
   * @param logFileDirectory [out] output variable that will contain path to log directory.
   */
  static void queryLogFileDirectory(bool forService,
                                    bool shareToAll,
                                    StringStorage *logFileDirectory);
private:
  /**
   * Deny instanizing of this class.
   */
  TvnLogFilename();
};

#endif
