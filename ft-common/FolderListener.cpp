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

#include "FolderListener.h"
#include <vector>

FolderListener::FolderListener(const TCHAR *folderPath)
: m_filesInfo(NULL), m_filesCount(0)
{
  m_folderPath.setString(folderPath);
}

FolderListener::~FolderListener()
{
  if (m_filesInfo != NULL) {
    delete[] m_filesInfo;
  }
}

const FileInfo *FolderListener::getFilesInfo() const
{
  return m_filesInfo;
}

UINT32 FolderListener::getFilesCount() const
{
  return m_filesCount;
}

bool FolderListener::list()
{
  StringStorage *fileNameList = NULL;
  m_filesCount = 0;
  bool listResult = true;

  if (!m_folderPath.isEmpty()) {
    File folder(m_folderPath.getString());
    listResult = folder.list(NULL, &m_filesCount);
  } else {
    File::listRoots(NULL, &m_filesCount);
  }

  if (!listResult) {
    return false;
  }

  fileNameList = new StringStorage[m_filesCount];

  if (m_filesInfo != NULL) {
    delete[] m_filesInfo;
    m_filesInfo = NULL;
  }

  m_filesInfo = new FileInfo[m_filesCount];

  if (!m_folderPath.isEmpty()) {
    File folder(m_folderPath.getString());
    folder.list(fileNameList, NULL);
  } else {
    File::listRoots(fileNameList, NULL);
  }

  for (UINT32 i = 0; i < m_filesCount; i++) {

    const TCHAR *fileName = fileNameList[i].getString();
    const TCHAR *folderName = m_folderPath.getString();

    File file(folderName, fileName);
    FileInfo fileInfo(&file);

    m_filesInfo[i] = fileInfo;

    //
    // All files in root folder is directories
    //

    if (m_folderPath.isEmpty()) {
      m_filesInfo[i].setFlags(FileInfo::DIRECTORY);
      m_filesInfo[i].setSize(0);
      m_filesInfo[i].setLastModified(0);
    }
  }

  delete[] fileNameList;

  return true;
}
