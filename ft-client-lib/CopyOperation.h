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

#ifndef _COPY_OPERATION_H_
#define _COPY_OPERATION_H_

#include "FileTransferOperation.h"
#include "CopyFileEventListener.h"

//
// Abstract class that contain intersection of functionality
// of Download and Upload operations.
//

class CopyOperation : public FileTransferOperation
{
public:
  CopyOperation(LogWriter *logWriter);

  virtual ~CopyOperation();

  // Sets copy event listener
  void setCopyProcessListener(CopyFileEventListener *listener);

protected:
  // Information about file that currently coping
  FileInfoList *m_toCopy;

  // Path to root folder where files that should be copied is located
  StringStorage m_pathToSourceRoot;
  // Path to target root folder where copied files will be located
  // after coping
  StringStorage m_pathToTargetRoot;

  // Path to source file that we currently coping
  StringStorage m_pathToSourceFile;
  // Path to target file in that we currently coping
  // source file
  StringStorage m_pathToTargetFile;

  // Listener of copy operation events
  CopyFileEventListener *m_copyListener;

  // Members that needed to know how many bytes of data transfered
  // and how many left to copy
  UINT64 m_totalBytesToCopy;
  UINT64 m_totalBytesCopied;
};

#endif
