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

#include "WinFileChannel.h"

WinFileChannel::WinFileChannel(const TCHAR *pathName,
                               DesiredAccess dAcc,
                               FileMode fMode,
                               bool sharedToRead)
{
  m_winFile.open(pathName, dAcc, fMode, sharedToRead);
}

WinFileChannel::~WinFileChannel()
{
  try { close(); } catch (...) {}
}

size_t WinFileChannel::read(void *buffer, size_t len)
{
  return m_winFile.read(buffer, len);
}

size_t WinFileChannel::write(const void *buffer, size_t len)
{
  return m_winFile.write(buffer, len);
}

void WinFileChannel::close()
{
  m_winFile.close();
}

void WinFileChannel::seek(INT64 n)
{
  try {
    m_winFile.seek(n);
  } catch (Exception &e) {
    throw IOException(e.getMessage());
  }
}

size_t WinFileChannel::available() {
  return m_winFile.available();
}
