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

#include "ScreenHooks.h"
#include "util/CommonHeader.h"
#include "tvnserver-app/NamingDefs.h"
#include "region/Point.h"
#include "region/Region.h"

// Pre-definition:
LRESULT CALLBACK callWndRetProc(int nCode, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK getMsgProc(int code, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK sysMsgProc(int code, WPARAM wParam, LPARAM lParam);
void processMessage(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
void sendRect(const Rect *rect);
void sendClientRect(HWND hwnd);
void sendNClientRegion(HWND hwnd);
Rect getWindowRect(HWND hwnd);
Rect getClientRect(HWND hwnd);

// Per-instance variables:
HMODULE g_hModule = 0;

#pragma comment(linker, "/section:.shared,RWS")
#pragma data_seg(".shared")
HHOOK g_callWndProcH = 0;
HHOOK g_getMessageH = 0;
HHOOK g_sysMessageH = 0;
HWND g_targetWinHwnd = 0;
#pragma data_seg()

BOOL APIENTRY DllMain(HMODULE hModule,
                      DWORD  ul_reason_for_call,
                      LPVOID lpReserved)
{
  switch (ul_reason_for_call)
  {
  case DLL_PROCESS_ATTACH:
    g_hModule = hModule;
    break;
  case DLL_THREAD_ATTACH:
  case DLL_THREAD_DETACH:
  case DLL_PROCESS_DETACH:
    break;
  }
  return TRUE;
}

__declspec(dllexport) bool setHook(HWND targedWinHwnd)
{
  g_targetWinHwnd = targedWinHwnd;
  g_callWndProcH = SetWindowsHookEx(WH_CALLWNDPROCRET,
                                    callWndRetProc,
                                    g_hModule,
                                    0);

  g_getMessageH = SetWindowsHookEx(WH_GETMESSAGE,
                                   getMsgProc,
                                   g_hModule,
                                   0);

  g_sysMessageH = SetWindowsHookEx(WH_SYSMSGFILTER,
                                   sysMsgProc,
                                   g_hModule,
                                   0);

  return g_callWndProcH != 0 && g_getMessageH != 0 && g_sysMessageH != 0;
}

__declspec(dllexport) bool unsetHook()
{
  bool result = UnhookWindowsHookEx(g_callWndProcH) != 0;
  result = result && UnhookWindowsHookEx(g_getMessageH) != 0;
  return result;
}

LRESULT CALLBACK callWndRetProc(int nCode, WPARAM wParam, LPARAM lParam)
{
  if (nCode == HC_ACTION) {
    CWPRETSTRUCT *cpwS = (CWPRETSTRUCT *)lParam;
    processMessage(cpwS->hwnd, cpwS->message, cpwS->wParam, cpwS->lParam);
  }
  return CallNextHookEx(g_callWndProcH, nCode, wParam, lParam);
}

LRESULT CALLBACK getMsgProc(int code, WPARAM wParam, LPARAM lParam)
{
  if (code == MSGF_DIALOGBOX || code == MSGF_MENU || code == MSGF_SCROLLBAR) {
    MSG *msg = (MSG *)lParam;
    processMessage(msg->hwnd, msg->message, msg->wParam, msg->lParam);
  }
  return CallNextHookEx(g_getMessageH, code, wParam, lParam);
}

LRESULT CALLBACK sysMsgProc(int code, WPARAM wParam, LPARAM lParam)
{
  if (code == MSGF_DIALOGBOX || code == MSGF_MENU || code == MSGF_SCROLLBAR) {
    MSG *msg = (MSG *)lParam;
    processMessage(msg->hwnd, msg->message, msg->wParam, msg->lParam);
  }
  return CallNextHookEx(g_sysMessageH, code, wParam, lParam);
}

void processMessage(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  switch (message) {
  case WM_PAINT:
    //FIXME: Process a region that folowing with WM_PAINT.
  case WM_CTLCOLOREDIT:
  case WM_ACTIVATE:
  case WM_ERASEBKGND:
    sendClientRect(hwnd);
    break;
  case WM_NCPAINT:
  case WM_NCACTIVATE:
  case WM_HSCROLL:
  case WM_VSCROLL:
    sendNClientRegion(hwnd);
    break;
  case 0x0092: // Menu
    sendClientRect(hwnd);
    sendNClientRegion(hwnd);
    break;
  case WM_DRAWITEM:
    if (lParam != 0) {
      DRAWITEMSTRUCT *dts = (DRAWITEMSTRUCT *)lParam;
      HWND ctlHwnd = dts->hwndItem;
      if (wParam == 0) { // it is a menu control
      } else {
        sendRect(&getWindowRect(ctlHwnd));
      }
    }
    break;
  }
}

void sendRect(const Rect *rect)
{
  INT16 left   = (INT16)rect->left;
  INT16 top    = (INT16)rect->top;
  INT16 right  = (INT16)rect->right;
  INT16 bottom = (INT16)rect->bottom;
  PostMessage(g_targetWinHwnd, HookDefinitions::SPEC_IPC_CODE,
              MAKEWPARAM(top, left),
              MAKELPARAM(bottom, right));
}

void sendClientRect(HWND hwnd)
{
  Rect clientRect = getClientRect(hwnd);
  sendRect(&clientRect);
}

void sendNClientRegion(HWND hwnd)
{
  Region ncRegion(&getWindowRect(hwnd));
  Region cRegion(&getClientRect(hwnd));

  ncRegion.subtract(&cRegion);

  std::vector<Rect> rects;
  std::vector<Rect>::iterator iRect;
  ncRegion.getRectVector(&rects);
  for (iRect = rects.begin(); iRect < rects.end(); iRect++) {
    sendRect(&(*iRect));
  }
}

Rect getWindowRect(HWND hwnd)
{
  Point offset;
  offset.x = GetSystemMetrics(SM_XVIRTUALSCREEN);
  offset.y = GetSystemMetrics(SM_YVIRTUALSCREEN);

  RECT clientRect;
  Rect resultRect;
  if (GetWindowRect(hwnd, &clientRect) != 0) {
    resultRect.fromWindowsRect(&clientRect);
    resultRect.move(offset.x, offset.y);
  }
  return resultRect;
}

Rect getClientRect(HWND hwnd)
{
  Rect resultRect;
  RECT clientRect;
  if (GetClientRect(hwnd, &clientRect) != 0) {
    POINT offset;
    offset.x = 0;
    offset.y = 0;

    ClientToScreen(hwnd, &offset);
    offset.x -= GetSystemMetrics(SM_XVIRTUALSCREEN);
    offset.y -= GetSystemMetrics(SM_YVIRTUALSCREEN);
    resultRect.fromWindowsRect(&clientRect);
    resultRect.move(offset.x, offset.y);
  }
  return resultRect;
}
