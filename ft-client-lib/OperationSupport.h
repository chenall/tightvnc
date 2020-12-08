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

#ifndef _OPERATION_SUPPORT_H_
#define _OPERATION_SUPPORT_H_

#include "util/inttypes.h"

#include <vector>

class OperationSupport
{
public:
  OperationSupport();
  OperationSupport(const std::vector<UINT32> &clientCodes,
                   const std::vector<UINT32> &serverCodes);
  virtual ~OperationSupport();

  bool isFileListSupported() const;
  bool isUploadSupported() const;
  bool isDownloadSupported() const;
  bool isRenameSupported() const;
  bool isRemoveSupported() const;
  bool isMkDirSupported() const;
  bool isCompressionSupported() const;
  bool isMD5Supported() const;
  bool isDirSizeSupported() const;

protected:
  static bool isSupport(const std::vector<UINT32> &codes, UINT32 code);

  bool m_isFileListSupported;
  bool m_isUploadSupported;
  bool m_isDownloadSupported;
  bool m_isRenameSupported;
  bool m_isRemoveSupported;
  bool m_isMkDirSupported;
  bool m_isCompressionSupported;
  bool m_isMD5Supported;
  bool m_isDirSizeSupported;
};

#endif
