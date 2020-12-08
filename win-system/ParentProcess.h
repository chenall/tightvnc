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

#ifndef __PARENTPROCESS_H__
#define __PARENTPROCESS_H__

#include "util/CommonHeader.h"
#include <Tlhelp32.h>

// Retrieves information of the parent process.
class ParentProcess
{
public:
  ParentProcess();
  virtual ~ParentProcess();

  // Returns true if the parent process is alive.
  bool isAlive();

private:
  void init();
  // Returns true and fills the *pe argument if a process entry has been found.
  bool getProcessEntry(PROCESSENTRY32 *peOut, DWORD Pid,
                        HANDLE hSnapshot);

  PROCESSENTRY32 m_parentPe;

  // Is true if the m_parentPe field was successfully initialized.
  bool m_isInitialized;

};

#endif // __PARENTPROCESS_H__
