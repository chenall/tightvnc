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

#include "Deflater.h"
#include <crtdbg.h>

Deflater::Deflater()
{
  m_zlibStream.zalloc = Z_NULL;
  m_zlibStream.zfree = Z_NULL;

  deflateInit(&m_zlibStream, Z_DEFAULT_COMPRESSION);

  m_zlibStream.next_in = 0;
  m_zlibStream.avail_in = 0;

  m_zlibStream.next_out = 0;
  m_zlibStream.avail_out = 0;
}

Deflater::~Deflater()
{
  deflateEnd(&m_zlibStream);
}

void Deflater::deflate()
{
  size_t reserve = m_inputSize / 100 + 1024;
  size_t avaliableOutput = m_inputSize + reserve;
  unsigned long prevTotalOut = m_zlibStream.total_out;

  unsigned int constrainedValue = (unsigned int)avaliableOutput;
  _ASSERT(avaliableOutput == constrainedValue);

  m_output.resize(avaliableOutput);

  m_zlibStream.next_in = (Bytef *)m_input;
  m_zlibStream.avail_in = (unsigned int)m_inputSize;

  m_zlibStream.next_out = (Bytef *)&m_output.front();
  m_zlibStream.avail_out = (unsigned int)avaliableOutput;

  if (::deflate(&m_zlibStream, Z_SYNC_FLUSH) != Z_OK) {
    throw ZLibException(_T("Deflate method return error"));
  }

  if (m_zlibStream.avail_in != 0) {
    throw ZLibException(_T("Not enough buffer size for data compression"));
  }
 
  m_outputSize = m_zlibStream.total_out - prevTotalOut;
}
