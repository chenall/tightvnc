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

#include "EmulatedAnonymousPipeFactory.h"
#include "win-system/SecurityAttributes.h"
#include "win-system/PipeServer.h"
#include "win-system/PipeClient.h"
#include <time.h>

EmulatedAnonymousPipeFactory::EmulatedAnonymousPipeFactory(unsigned int bufferSize, LogWriter *log)
: m_bufferSize(bufferSize),
  m_log(log)
{
}

EmulatedAnonymousPipeFactory::~EmulatedAnonymousPipeFactory()
{
}

void EmulatedAnonymousPipeFactory::generatePipes(NamedPipe **serverPipe, bool serverInheritable,
                                                 NamedPipe **clientPipe, bool clientInheritable)
{
  SecurityAttributes secAttr;
  secAttr.setInheritable();

  StringStorage randomName;
  getUniqPipeName(&randomName);
  PipeServer pipeServer(randomName.getString(), m_bufferSize, 0, 1000);
  *clientPipe = PipeClient::connect(randomName.getString(), m_bufferSize);
  *serverPipe = pipeServer.accept();

  HANDLE hThisSideWrite = (*serverPipe)->getHandle();
  HANDLE hOtherSideRead = (*clientPipe)->getHandle();

  const TCHAR *errMess = _T("Cannot disable inheritance for named pipe");
  if (!serverInheritable) {
    if (SetHandleInformation(hThisSideWrite, HANDLE_FLAG_INHERIT, 0) == 0) {
      SystemException(errMess);
    }
  }
  if (!clientInheritable) {
    if (SetHandleInformation(hOtherSideRead, HANDLE_FLAG_INHERIT, 0) == 0) {
      SystemException(errMess);
    }
  }
}

void EmulatedAnonymousPipeFactory::getUniqPipeName(StringStorage *result)
{
  srand((unsigned)time(0));
  for (int i = 0; i < 20; i++) {
    result->appendChar('a' + rand() % ('z' - 'a'));
  }
}
