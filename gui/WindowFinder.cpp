// Copyright (C) 2008,2009,2010,2011,2012 GlavSoft LLC.
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

#include "WindowFinder.h"

struct WindowsParam
{
  std::vector<HWND> *hwndVector;
  StringVector *classNames;
};

BOOL CALLBACK WindowFinder::findWindowsByClassFunc(HWND hwnd, LPARAM lParam)
{
  if (IsWindowVisible(hwnd) != 0) {
    WindowsParam *windowsParam = (WindowsParam *)lParam;
    StringVector::iterator classNameIter;

    const size_t maxTcharCount = 256;
    TCHAR winName[maxTcharCount];
    if (GetClassName(hwnd, winName, maxTcharCount) != 0) {
      StringStorage nextWinName(winName);

      if (nextWinName.getLength() > 0 && hwnd != 0) {
        for (classNameIter = windowsParam->classNames->begin();
             classNameIter != windowsParam->classNames->end(); classNameIter++) {
          if (nextWinName.isEqualTo(&(*classNameIter))) {
            windowsParam->hwndVector->push_back(hwnd);
          }
        }
      }

      // Recursion
      EnumChildWindows(hwnd, findWindowsByClassFunc, (LPARAM) windowsParam);
    }
  }
  return TRUE;
}

std::vector<HWND> WindowFinder::findWindowsByClass(StringVector classNames)
{
  std::vector<HWND> hwndVector;
  if (classNames.empty()) {
    return hwndVector;
  }
  WindowsParam windowsParam;
  windowsParam.classNames = &classNames;
  windowsParam.hwndVector = &hwndVector;
  EnumWindows(findWindowsByClassFunc, (LPARAM)&windowsParam);
  return hwndVector;
}

BOOL CALLBACK WindowFinder::findWindowsByNameFunc(HWND hwnd, LPARAM lParam)
{
  if (IsWindowVisible(hwnd) != 0) {
    const size_t maxTcharCount = 256;
    TCHAR nameChars[maxTcharCount];
    if (GetWindowText(hwnd, nameChars, maxTcharCount) != 0) {
      StringStorage winName(nameChars);
      winName.toLowerCase();

      if (winName.getLength() > 0 && hwnd != 0) {
        WindowsParam *winParams = (WindowsParam *)lParam;
        StringStorage *substr = &(*(winParams->classNames)).front();
        if (_tcsstr(winName.getString(), substr->getString()) != 0) {
          (*(winParams->hwndVector)).push_back(hwnd);
          return FALSE;
        }
      }
    }
  }
  return TRUE;
}

HWND WindowFinder::findFirstWindowByName(const StringStorage windowName)
{
  std::vector<HWND> hwndVector;
  StringVector winNameVector;
  winNameVector.push_back(windowName);
  winNameVector[0].toLowerCase();
  WindowsParam winParams = { &hwndVector, &winNameVector };

  EnumWindows(findWindowsByNameFunc, (LPARAM)&winParams);
  if (hwndVector.size() != 0) {
    return hwndVector[0];
  } else {
    return 0;
  }
}
