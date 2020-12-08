// Copyright (C) 2012 GlavSoft LLC.
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

#include "WinClipboard.h"

WinClipboard::WinClipboard(HWND hwnd) 
  : m_hndClipboard(0), m_hWnd(hwnd)
{
}

WinClipboard::~WinClipboard() 
{
  if (m_hndClipboard) {
    EmptyClipboard();
    GlobalFree(m_hndClipboard);
  }
}

void WinClipboard::setHWnd(HWND hwnd)
{
  m_hWnd = hwnd;
  SetClipboardViewer(m_hWnd);
}

bool WinClipboard::getString(StringStorage *str)
{
  UINT strType = CF_UNICODETEXT;

  if (sizeof(TCHAR) == 1) {
    strType = CF_TEXT;
  }
  int uFormat = GetPriorityClipboardFormat(&strType, sizeof(UINT));

  if (uFormat == 0 || uFormat == -1) {
     return false;
  }
  if (OpenClipboard(m_hWnd)) {
     HANDLE hndData = GetClipboardData(uFormat); 

     if (hndData) {
        TCHAR *szData = (TCHAR *)GlobalLock(hndData); 
        StringStorage nativeClipboard = szData;
        //str->setString(szData);
        GlobalUnlock(hndData); 
        CloseClipboard();
        *str = removeCR(&nativeClipboard);
        return true;
      }
    CloseClipboard();
    return false;
  }
  return false;
}

bool WinClipboard::setString(const StringStorage *serverClipboard)
{
  StringStorage nativeClipboard = addCR(serverClipboard);

  int dataType = CF_UNICODETEXT;

  if (sizeof(TCHAR) == 1) {
     dataType = CF_TEXT;
  }
  int strLength = static_cast<int>(nativeClipboard.getLength()) + 1;
  int dataSize = strLength * sizeof(TCHAR);

  if (OpenClipboard(m_hWnd)) {
     if (m_hndClipboard) {
         EmptyClipboard();
         GlobalFree(m_hndClipboard);
      }
      m_hndClipboard = GlobalAlloc(GMEM_MOVEABLE, dataSize);
      CopyMemory(GlobalLock(m_hndClipboard), nativeClipboard.getString(), dataSize);
      GlobalUnlock(m_hndClipboard);
      SetClipboardData(dataType, m_hndClipboard);
      CloseClipboard();
      return true;
  }
  return false;
}

StringStorage WinClipboard::addCR(const StringStorage *str)
{
  const TCHAR *beginString = str->getString();
  const TCHAR *endString = beginString + str->getLength() + 1; // start + lenght + '\0'
  vector<TCHAR> chars(beginString, endString);
  vector<TCHAR> newChars(str->getLength() * 2 + 1);
  size_t countLF = 0;
  for (size_t i = 0; i < chars.size(); i++) {
    // if is first byte or previous byte not CR, then add CR
    if ((i == 0 || chars[i-1] != CR) && chars[i] == LF) {
      newChars[i + countLF] = CR;
      ++countLF;
    }
    newChars[i + countLF] = chars[i];
  }
  newChars.resize(chars.size() + countLF);
  return StringStorage(&newChars.front());
}

StringStorage WinClipboard::removeCR(const StringStorage *str)
{
  const TCHAR *beginString = str->getString();
  const TCHAR *endString = beginString + str->getLength() + 1; // start + lenght + '\0'
  vector<TCHAR> chars(beginString, endString);
  vector<TCHAR> newChars;
  size_t countLF = 0;
  for (size_t i = 0; i < chars.size(); i++) {
    if (chars[i] != CR || i + 1 == chars.size() || chars[i+1] != LF) {
      newChars.push_back(chars[i]);
    }
  }
  return StringStorage(&newChars.front());
}
