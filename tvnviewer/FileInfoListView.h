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

#ifndef _FILE_INFO_LIST_VIEW_H_
#define _FILE_INFO_LIST_VIEW_H_

#include "gui/ListView.h"
#include "ft-common/FileInfo.h"

class FileInfoListView : public ListView
{
public:
  FileInfoListView();
  virtual ~FileInfoListView();

  virtual void setWindow(HWND hwnd);

  //
  // Adds new item to FileInfoListView
  //

  void addItem(int index, FileInfo *fileInfo);

  //
  // Adds files info array to the end of list view
  //

  void addRange(FileInfo **filesInfo, size_t count);

  //
  // Returns file info notated by first selected list view item
  //

  FileInfo *getSelectedFileInfo();

  void sort(int columnIndex);
protected:

  //
  // Loads file list view icons from application resources
  //

  void loadImages();

  //
  // This function compare two item with file-contex (file name, date, size).
  //
  static int CALLBACK compareItem(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);

  //
  // This function return:
  //   -1, if first < second
  //   0, if first == second
  //   1, if first > second
  //
  static int compareUInt64(UINT64 first, UINT64 second);

  HIMAGELIST m_smallImageList;

private:
  static LRESULT CALLBACK s_newWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

  static const int IMAGE_FOLDER_UP_INDEX = 0;
  static const int IMAGE_FOLDER_INDEX = 1;
  static const int IMAGE_FILE_INDEX = 2;
};

#endif
