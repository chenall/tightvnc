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

#include "AnonymousPipeFactory.h"
#include "win-system/SecurityAttributes.h"

AnonymousPipeFactory::AnonymousPipeFactory(unsigned int bufferSize,
                                           LogWriter *log)
: m_bufferSize(bufferSize),
  m_log(log)
{
}

AnonymousPipeFactory::~AnonymousPipeFactory()
{
}

void AnonymousPipeFactory::generatePipes(AnonymousPipe **firstSide,
                                         bool firstSideIsInheritable,
                                         AnonymousPipe **secondSide,
                                         bool secondSideIsInheritable)
{
  HANDLE hFirstSideWrite = 0, hFirstSideRead = 0,
         hSecondSideWrite = 0, hSecondSideRead = 0;

  SecurityAttributes secAttr;
  secAttr.setInheritable();

  try {
    if (CreatePipe(&hFirstSideRead, &hSecondSideWrite,
                   secAttr.getSecurityAttributes(), m_bufferSize) == 0) {
      SystemException(_T("Cannot create anonymous pipe"));
    }
    if (CreatePipe(&hSecondSideRead, &hFirstSideWrite,
                   secAttr.getSecurityAttributes(), m_bufferSize) == 0) {
      SystemException(_T("Cannot create anonymous pipe"));
    }
  } catch (...) {
    CloseHandle(hFirstSideWrite);
    CloseHandle(hFirstSideRead);
    CloseHandle(hSecondSideWrite);
    CloseHandle(hSecondSideRead);
    throw;
  }

  const TCHAR *errMess = _T("Cannot disable inheritance for anonymous pipe");
  if (!firstSideIsInheritable) {
    if (SetHandleInformation(hFirstSideWrite, HANDLE_FLAG_INHERIT, 0) == 0) {
      SystemException(errMess);
    }
    if (SetHandleInformation(hFirstSideRead, HANDLE_FLAG_INHERIT, 0) == 0) {
      SystemException(errMess);
    }
  }
  if (!secondSideIsInheritable) {
    if (SetHandleInformation(hSecondSideWrite, HANDLE_FLAG_INHERIT, 0) == 0) {
      SystemException(errMess);
    }
    if (SetHandleInformation(hSecondSideRead, HANDLE_FLAG_INHERIT, 0) == 0) {
      SystemException(errMess);
    }
  }

  *firstSide = new AnonymousPipe(hFirstSideWrite, hFirstSideRead, m_bufferSize, m_log);
  *secondSide = new AnonymousPipe(hSecondSideWrite, hSecondSideRead, m_bufferSize, m_log);
}
