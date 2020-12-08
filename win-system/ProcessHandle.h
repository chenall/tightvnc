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

#include "util/COmmonHeader.h"

// This class is a mere envelop for process handle that will automatically
// closed at destructor calling.
class ProcessHandle
{
public:
  ProcessHandle();
  virtual ~ProcessHandle();

  // @throws Exception on an error.
  void openProcess(DWORD dwDesiredAccess,
                   BOOL bInheritHandle,
                   DWORD dwProcessId);

  // Returns the handle of the openned process by openProcess() function.
  // If openProcess() function has not been called before then getHandle()
  // will return zero.
  HANDLE getHandle() const;

  // Returns process module path. Call the openProcess() function before.
  // @throws Exception on an error.
  void getProcessModulePath(StringStorage *exePath);

private:
  HANDLE m_hProcess;
};
