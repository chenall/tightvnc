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

#include "FileInfoListView.h"
#include "util/DateTime.h"
#include "tvnviewer/resource.h"
#include "util/ResourceLoader.h"
#include <crtdbg.h>
#include <stdio.h>

FileInfoListView::FileInfoListView()
: m_smallImageList(0)
{
  // By default, file list is sorted by file name.
  sort(0);
}

FileInfoListView::~FileInfoListView()
{
  if (m_smallImageList != 0) {
    ImageList_Destroy(m_smallImageList);
  }
}

//
// Saves hwnd and automaticly adds columns to list view
//

void FileInfoListView::setWindow(HWND hwnd)
{
  ListView::setWindow(hwnd);

  ListView::addColumn(0, _T("Name"), 135);
  ListView::addColumn(1, _T("Size"), 80, LVCFMT_RIGHT);
  ListView::addColumn(2, _T("Modified"), 115);

  setFullRowSelectStyle(true);

  loadImages();

  ListView_SetImageList(m_hwnd, m_smallImageList, LVSIL_SMALL);

  Control::replaceWindowProc(FileInfoListView::s_newWndProc);
}

void FileInfoListView::addItem(int index, FileInfo *fileInfo)
{
  const TCHAR *filename = fileInfo->getFileName();

  int imageIndex = IMAGE_FILE_INDEX;

  if (_tcscmp(fileInfo->getFileName(), _T("..")) == 0) {
    imageIndex = IMAGE_FOLDER_UP_INDEX;
  } else if (fileInfo->isDirectory()) {
    imageIndex = IMAGE_FOLDER_INDEX;
  }

  ListView::addItem(index, filename, (LPARAM)fileInfo, imageIndex);

  StringStorage sizeString(_T("<Folder>"));
  StringStorage modTimeString(_T(""));

  if (!fileInfo->isDirectory()) {
    //
    // Prepare size string
    //

    UINT64 fileSize = fileInfo->getSize();

    if (fileSize <= 1024) {
      sizeString.format(_T("%ld B"), fileSize);
    } else if ((fileSize > 1024) && (fileSize <= 1024 * 1024)) {
      sizeString.format(_T("%4.2f KB"), static_cast<double>(fileSize) / 1024.0);
    } else if (fileSize > 1024 * 1024) {
      sizeString.format(_T("%4.2f MB"), static_cast<double>(fileSize) / (1024.0 * 1024));
    }

    //
    // Prepare modification time string
    //

    DateTime dateTime(fileInfo->lastModified());

    dateTime.toString(&modTimeString);
  }

  ListView::setSubItemText(index, 1, sizeString.getString());
  ListView::setSubItemText(index, 2, modTimeString.getString());
  ListView::sort();
}

void FileInfoListView::addRange(FileInfo **filesInfo, size_t count)
{
  int index = max(0, (getCount() - 1));
  size_t i = 0;
  FileInfo *arr = *filesInfo;

  // Add folders first
  for (i = 0; i < count; i++) {
    FileInfo *fi = &arr[i];
    if (fi->isDirectory()) {
      addItem(index++, fi);
    } // if directory
  } // for all files info

  // Add files
  for (i = 0; i < count; i++) {
    FileInfo *fi = &arr[i];
    if (!fi->isDirectory()) {
      addItem(index++, fi);
    } // if not directory
  } // for all files info

  ListView::sort();
} // void

FileInfo *FileInfoListView::getSelectedFileInfo()
{
  int si = getSelectedIndex();
  if (si == -1) {
    return NULL;
  }
  return reinterpret_cast<FileInfo *>(getSelectedItem().tag);
}

void FileInfoListView::loadImages()
{
  if (m_smallImageList != NULL) {
    ImageList_Destroy(m_smallImageList);
  }

  m_smallImageList = ImageList_Create(GetSystemMetrics(SM_CXSMICON),
                                      GetSystemMetrics(SM_CYSMICON),
                                      ILC_MASK, 1, 1);

  HICON icon;

  ResourceLoader *rLoader = ResourceLoader::getInstance();

  icon = rLoader->loadIcon(MAKEINTRESOURCE(IDI_FILEUP));
  _ASSERT(icon != NULL);
  ImageList_AddIcon(m_smallImageList, icon);
  DestroyIcon(icon);

  icon = rLoader->loadIcon(MAKEINTRESOURCE(IDI_FOLDER_ICON));
  _ASSERT(icon != NULL);
  ImageList_AddIcon(m_smallImageList, icon);
  DestroyIcon(icon);

  icon = rLoader->loadIcon(MAKEINTRESOURCE(IDI_FILE_ICON));
  _ASSERT(icon != NULL);
  ImageList_AddIcon(m_smallImageList, icon);
  DestroyIcon(icon);
}

void FileInfoListView::sort(int columnIndex)
{
  ListView::sort(columnIndex, compareItem);
}

int FileInfoListView::compareUInt64(UINT64 first, UINT64 second)
{
  if (first < second) {
    return -1;
  }
  if (first > second) {
    return 1;
  }
  return 0;
}

int FileInfoListView::compareItem(LPARAM lParam1,
                                   LPARAM lParam2,
                                   LPARAM lParamSort)
{
  // check ascending order
  bool sortAscending = lParamSort > 0;

  FileInfo *firstItem = reinterpret_cast<FileInfo *>(lParam1);
  FileInfo *secondItem = reinterpret_cast<FileInfo *>(lParam2);

  // Fake directory ".." should be into top list.
  if (_tcscmp(firstItem->getFileName(), _T("..")) == 0) {
    return -1;
  }

  if (_tcscmp(secondItem->getFileName(), _T("..")) == 0) {
    return 1;
  }

  // Directories should be upper, than files.
  if (firstItem->isDirectory() && !secondItem->isDirectory()) {
    return -1;
  }
  if (!firstItem->isDirectory() && secondItem->isDirectory()) {
    return 1;
  }
  
  // change lParam1 and lParam2 with each other if order is descending
  if (sortAscending) {
    firstItem = reinterpret_cast<FileInfo *>(lParam1);
    secondItem = reinterpret_cast<FileInfo *>(lParam2);
  } else {
    firstItem = reinterpret_cast<FileInfo *>(lParam2);
    secondItem = reinterpret_cast<FileInfo *>(lParam1);
  }

  if (lParamSort < 0) {
    // calculate column number when order is descending
    lParamSort = abs(lParamSort) - 1;
  } else {
    // calculate column number when order is ascending
    lParamSort -= 1;
  }
 
  switch (lParamSort) {
  // It's column "FileName".
  case 0:
    return _tcsicmp(firstItem->getFileName(), secondItem->getFileName());

  // It's column "FileSize".
  case 1:
    {
      // Size of directory is 0. Sort him by name.
      if (firstItem->isDirectory()) {
        return compareItem(lParam1, lParam2, 1);
      }
      int compareSize = compareUInt64(firstItem->getSize(), secondItem->getSize());
      // Sort by name, if sizes is equal.
      if (compareSize == 0) {
        return compareItem(lParam1, lParam2, 1);
      }
      return compareSize;
    }
  // It's column "Last modified".
  case 2:
    {
      int compareTime = compareUInt64(firstItem->lastModified(), secondItem->lastModified());
      // Sort by name, if time stamps is equal.
      if (compareTime == 0) {
        return compareItem(lParam1, lParam2, 1);
      }
      return compareTime;
    }
  // It's unknown column.
  default:
    _ASSERT(false);
    return 0;
  }
}

LRESULT CALLBACK FileInfoListView::s_newWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  FileInfoListView *_this = reinterpret_cast<FileInfoListView *>(GetWindowLongPtr(hwnd, GWLP_USERDATA));

  switch (uMsg) {
  case WM_GETDLGCODE:
    LRESULT lres = CallWindowProc(_this->m_defWindowProc, hwnd, uMsg, wParam, lParam);
    // We want WM_KEYDOWN message when enter is pressed
    if (lParam &&
        ((MSG *)lParam)->message == WM_KEYDOWN &&
        ((MSG *)lParam)->wParam == VK_RETURN) {
      lres = DLGC_WANTMESSAGE;
    }
    return lres;
  } // switch

  return CallWindowProc(_this->m_defWindowProc, hwnd, uMsg, wParam, lParam);
}
