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

#include "DataOutputStream.h"
#include "util/Utf8StringStorage.h"
#include <vector>

#define GETBYTE(x, n) (((x) >> ((n) * 8)) & 0xFF)

DataOutputStream::DataOutputStream(OutputStream *outputStream)
: m_outStream(outputStream)
{
}

DataOutputStream::~DataOutputStream()
{
}

size_t DataOutputStream::write(const void *buffer, size_t len)
{
  return m_outStream->write(buffer, len);
}

void DataOutputStream::writeFully(const void *buffer, size_t len)
{
  char *typedBuffer = (char *)buffer;
  size_t totalWritten = 0;
  size_t left = len;
  while (totalWritten < len) {
    size_t written = m_outStream->write(typedBuffer + totalWritten, left);
    left -= written;
    totalWritten += written;
  }
}

void DataOutputStream::writeUInt8(UINT8 x)
{
  writeFully((char *)&x, 1);
};

void DataOutputStream::writeUInt16(UINT16 data)
{
  UINT8 buf[2];

  buf[0] = GETBYTE(data, 1);
  buf[1] = GETBYTE(data, 0);

  writeFully((char *)buf, sizeof(buf));
}

void DataOutputStream::writeUInt32(UINT32 data)
{
  UINT8 buf[4];

  buf[0] = GETBYTE(data, 3);
  buf[1] = GETBYTE(data, 2);
  buf[2] = GETBYTE(data, 1);
  buf[3] = GETBYTE(data, 0);

  writeFully((char *)buf, sizeof(buf));
}

void DataOutputStream::writeUInt64(UINT64 data)
{
  UINT8 buf[8];

  buf[0] = GETBYTE(data, 7);
  buf[1] = GETBYTE(data, 6);
  buf[2] = GETBYTE(data, 5);
  buf[3] = GETBYTE(data, 4);
  buf[4] = GETBYTE(data, 3);
  buf[5] = GETBYTE(data, 2);
  buf[6] = GETBYTE(data, 1);
  buf[7] = GETBYTE(data, 0);

  writeFully((char *)buf, sizeof(buf));
}

void DataOutputStream::writeInt8(INT8 x)
{
  writeUInt8((UINT8)x);
}

void DataOutputStream::writeInt16(INT16 x)
{
  writeUInt16((UINT16)x);
}

void DataOutputStream::writeInt32(INT32 x)
{
  writeUInt32((UINT32)x);
}

void DataOutputStream::writeInt64(INT64 x)
{
  writeUInt64((UINT64)x);
}

void DataOutputStream::writeUTF8(const TCHAR *string)
{
  size_t sizeInBytes = 0;

  // to UTF8 string convertion
  Utf8StringStorage utf8String(&StringStorage(string));

  // FIXME: Why try/catch() is used?
  try {
    unsigned int sizeInBytes = (unsigned int)utf8String.getSize();
    _ASSERT(sizeInBytes == utf8String.getSize());
    writeUInt32(sizeInBytes);
    writeFully(utf8String.getString(), sizeInBytes);
  } catch (...) {
    throw;
  }
}

void DataOutputStream::flush()
{
  m_outStream->flush();
}
