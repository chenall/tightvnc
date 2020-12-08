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

#ifndef _COPY_FILE_EVENT_LISTENER_H_
#define _COPY_FILE_EVENT_LISTENER_H_

#include "util/inttypes.h"
#include "ft-common/FileInfo.h"

class CopyFileEventListener
{
public:

  friend class CopyOperation;
  friend class UploadOperation;
  friend class DownloadOperation;

  static const int TFE_OVERWRITE = 0x0;
  static const int TFE_SKIP      = 0x1;
  static const int TFE_APPEND    = 0x2;
  static const int TFE_CANCEL    = 0x3;

protected:

  //
  // Method called by copy operation when data chunk of source file is copied
  // to target file.
  //
  // Parameters:
  //
  // totalBytesCopied - total length of data in bytes that copied to target
  // file system during copy operation execution
  //
  // totalBytesToCopy - total length of data in bytes that must be copied to target
  // file system during copy operation execution
  //

  virtual void dataChunkCopied(UINT64 totalBytesCopied, UINT64 totalBytesToCopy) = 0;

  //
  // Method called by copy operation when some file must be copied,
  // but target file with such name already exists and operation doesn't
  // know what to do file target file.
  //
  // Next operation activity depends on this method returning value.
  //
  // Valid returning values:
  //
  // TFE_OVERWRITE - overwrite
  // TFE_SKIP - skip
  // TFE_APPEND - append
  // TFE_CANCEL - terminate operation
  //
  // Remark: method must return valid value (see above), or assertion will
  // fail.
  //

  virtual int targetFileExists(FileInfo *sourceFileInfo,
                               FileInfo *targetFileInfo,
                               const TCHAR *pathToTargetFile) = 0;
};

#endif
