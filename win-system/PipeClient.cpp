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

#include "PipeClient.h"
#include "util/Exception.h"

PipeClient::PipeClient()
{
}

NamedPipe *PipeClient::connect(const TCHAR *name, unsigned int maxPortionSize)
{
  StringStorage pipeName;
  pipeName.format(_T("\\\\.\\pipe\\%s"), name);

  HANDLE hPipe;
  hPipe = CreateFile(pipeName.getString(),  // pipe name
                     GENERIC_READ |         // read and write access
                     GENERIC_WRITE,
                     0,                     // no sharing
                     NULL,                  // default security attributes
                     OPEN_EXISTING,         // opens existing pipe
                     FILE_FLAG_OVERLAPPED,  // asynchronous mode
                     NULL);                 // no template file

  if (hPipe == INVALID_HANDLE_VALUE) {
    int errCode = GetLastError();
    StringStorage errMess;
    errMess.format(_T("Connect to pipe server failed, error code = %d"), errCode);
    throw Exception(errMess.getString());
  }

  DWORD dwMode = PIPE_READMODE_BYTE;
  if (!SetNamedPipeHandleState(hPipe,   // pipe handle
                               &dwMode,   // new pipe mode
                               NULL,      // don't set maximum bytes
                               NULL)      // don't set maximum time
                               ) {
    int errCode = GetLastError();
    StringStorage errMess;
    errMess.format(_T("SetNamedPipeHandleState failed, error code = %d"), errCode);
    throw Exception(errMess.getString());
  }

  return new NamedPipe(hPipe, maxPortionSize, false);
}
