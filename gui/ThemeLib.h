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

#ifndef _THEME_LIB_H_
#define _THEME_LIB_H_

#include "util/CommonHeader.h"

// XP specific stuff

#include <uxtheme.h>
#include <vssym32.h>

//
// Define type for api functions
//

typedef HRESULT (__stdcall *PFNCLOSETHEMEDATA)(HTHEME hTheme);

typedef HRESULT (__stdcall *PFNDRAWTHEMEBACKGROUND)(HTHEME hTheme, HDC hdc,
                                                    int iPartId, int iStateId,
                                                    const RECT *pRect,
                                                    const RECT *pClipRect);

typedef HTHEME (__stdcall *PFNOPENTHEMEDATA)(HWND hwnd, LPCWSTR pszClassList);

typedef HRESULT (__stdcall *PFNDRAWTHEMETEXT)(HTHEME hTheme, HDC hdc, int iPartId,
                                              int iStateId, LPCWSTR pszText,
                                              int iCharCount, DWORD dwTextFlags,
                                              DWORD dwTextFlags2, const RECT *pRect);

typedef HRESULT (__stdcall *PFNGETTHEMEBACKGROUNDCONTENTRECT)(HTHEME hTheme,  HDC hdc,
                                                              int iPartId, int iStateId,
                                                              const RECT *pBoundingRect,
                                                              RECT *pContentRect);

class ThemeLib
{
public:
  ThemeLib();
  ~ThemeLib();

  static bool initialize();
  static void deinitialize();

  static bool isLoaded();

  static HRESULT CloseThemeData(HTHEME hTheme);

  static HRESULT DrawThemeBackground(HTHEME hTheme, HDC hdc,
                                     int iPartId, int iStateId,
                                     const RECT *pRect,
                                     const RECT *pClipRect);

  static HTHEME OpenThemeData(HWND hwnd, LPCWSTR pszClassList);

  static HRESULT DrawThemeText(HTHEME hTheme, HDC hdc, int iPartId,
                               int iStateId, LPCWSTR pszText,
                               int iCharCount, DWORD dwTextFlags,
                               DWORD dwTextFlags2, const RECT *pRect);

  static HRESULT GetThemeBackgroundContentRect(HTHEME hTheme,  HDC hdc,
                                               int iPartId, int iStateId,
                                               const RECT *pBoundingRect,
                                               RECT *pContentRect);

private:
  static bool s_isThemeLibLoaded;
  static HMODULE s_lib;

  static PFNOPENTHEMEDATA s_OpenThemeData;
  static PFNDRAWTHEMEBACKGROUND s_DrawThemeBackground;
  static PFNCLOSETHEMEDATA s_CloseThemeData;
  static PFNDRAWTHEMETEXT s_DrawThemeText;
  static PFNGETTHEMEBACKGROUNDCONTENTRECT s_GetThemeBackgroundContentRect;
};

#endif
