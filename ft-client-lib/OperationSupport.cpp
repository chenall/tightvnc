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

#include "OperationSupport.h"

#include "ft-common/FTMessage.h"

#include <algorithm>

OperationSupport::OperationSupport()
{
  m_isFileListSupported = false;
  m_isRenameSupported = false;
  m_isRemoveSupported = false;
  m_isMkDirSupported = false;
  m_isCompressionSupported = false;
  m_isMD5Supported = false;
  m_isDirSizeSupported = false;
  m_isUploadSupported = false;
  m_isDownloadSupported = false;
}

OperationSupport::OperationSupport(const std::vector<UINT32> &clientCodes,
                                   const std::vector<UINT32> &serverCodes)
{
  m_isFileListSupported = isSupport(clientCodes, FTMessage::FILE_LIST_REQUEST) &&
                          isSupport(serverCodes, FTMessage::FILE_LIST_REPLY);

  m_isRenameSupported = isSupport(clientCodes, FTMessage::RENAME_REQUEST) &&
                         isSupport(serverCodes, FTMessage::RENAME_REPLY);

  m_isRemoveSupported = isSupport(clientCodes, FTMessage::REMOVE_REQUEST) &&
                         isSupport(serverCodes, FTMessage::REMOVE_REPLY) &&
                         m_isFileListSupported;

  m_isMkDirSupported = isSupport(clientCodes, FTMessage::MKDIR_REQUEST) &&
                        isSupport(serverCodes, FTMessage::MKDIR_REPLY);

  m_isCompressionSupported = isSupport(clientCodes, FTMessage::COMPRESSION_SUPPORT_REQUEST) &&
                              isSupport(serverCodes, FTMessage::COMPRESSION_SUPPORT_REPLY);

  m_isMD5Supported = isSupport(clientCodes, FTMessage::MD5_REQUEST) &&
                      isSupport(serverCodes, FTMessage::MD5_REPLY);

  m_isDirSizeSupported = isSupport(clientCodes, FTMessage::DIRSIZE_REQUEST) &&
                          isSupport(serverCodes, FTMessage::DIRSIZE_REPLY);

  m_isUploadSupported = isSupport(clientCodes, FTMessage::UPLOAD_START_REQUEST) &&
                         isSupport(clientCodes, FTMessage::UPLOAD_DATA_REQUEST) &&
                         isSupport(clientCodes, FTMessage::UPLOAD_END_REQUEST) &&
                         isSupport(serverCodes, FTMessage::UPLOAD_START_REPLY) &&
                         isSupport(serverCodes, FTMessage::UPLOAD_DATA_REPLY) &&
                         isSupport(serverCodes, FTMessage::UPLOAD_END_REPLY) &&
                         m_isMkDirSupported && m_isFileListSupported;

  m_isDownloadSupported = (isSupport(clientCodes, FTMessage::DOWNLOAD_START_REQUEST) &&
                           isSupport(clientCodes, FTMessage::DOWNLOAD_DATA_REQUEST) &&
                           isSupport(serverCodes, FTMessage::DOWNLOAD_START_REPLY) &&
                           isSupport(serverCodes, FTMessage::DOWNLOAD_DATA_REPLY) &&
                           isSupport(serverCodes, FTMessage::DOWNLOAD_END_REPLY) &&
                           m_isFileListSupported && m_isDirSizeSupported);
}

OperationSupport::~OperationSupport()
{
}

bool OperationSupport::isFileListSupported() const
{
  return m_isFileListSupported;
}

bool OperationSupport::isUploadSupported() const
{
  return m_isUploadSupported;
}

bool OperationSupport::isDownloadSupported() const
{
  return m_isDownloadSupported;
}

bool OperationSupport::isRenameSupported() const
{
  return m_isRenameSupported;
}

bool OperationSupport::isRemoveSupported() const
{
  return m_isRemoveSupported;
}

bool OperationSupport::isMkDirSupported() const
{
  return m_isMkDirSupported;
}

bool OperationSupport::isCompressionSupported() const
{
  return m_isCompressionSupported;
}

bool OperationSupport::isMD5Supported() const
{
  return m_isMD5Supported;
}

bool OperationSupport::isDirSizeSupported() const
{
  return m_isDirSizeSupported;
}

bool OperationSupport::isSupport(const std::vector<UINT32> &codes, UINT32 code)
{
  return std::find(codes.begin(), codes.end(), code) != codes.end();
}
