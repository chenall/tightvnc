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

#ifndef _CONTROL_PIPE_NAME_H_
#define _CONTROL_PIPE_NAME_H_

#include "util/CommonHeader.h"
#include "log-writer/LogWriter.h"

/**
 * Used to create pipe name for control transport.
 */
class ControlPipeName
{
public:
  /**
   * Creates pipe name for control transport.
   * @param forService flag determinating if we create pipe name for service or application.
   * @param pipeName [out] target pipe name.
   */
  static void createPipeName(bool forService, StringStorage *pipeName, LogWriter *log);

private:
  /**
   * Don't allow instanizing of this class.
   */
  ControlPipeName() { };
};

#endif
