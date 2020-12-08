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

#include "NamingDefs.h"

const TCHAR NamingDefs::MIRROR_DRIVER_MESSAGE_WINDOW_CLASS_NAME[] =
  _T("MIRROR_DRIVER_MESSAGE_WINDOW_CLASS_NAME");

const TCHAR ProductNames::PRODUCT_NAME[] = _T("TightVNC");
const TCHAR ProductNames::SERVER_PRODUCT_NAME[] = _T("TightVNC Server");

const TCHAR RegistryPaths::SERVER_PATH[] = _T("Software\\TightVNC\\Server");
const TCHAR RegistryPaths::SERVER_REVERSE_CONN_HISTORY_PATH[] =
  _T("Software\\TightVNC\\Control\\ReverseConnectionHistory");
const TCHAR RegistryPaths::DISPATCHER_CONN_HISTORY_PATH[] =
  _T("Software\\TightVNC\\Control\\DispatcherConnectionHistory");

const TCHAR WindowNames::WINDOW_CLASS_NAME[] =
  _T("TvnWindowsApplicationClass");

const TCHAR LogNames::SERVER_LOG_FILE_STUB_NAME[] = _T("tvnserver");
const TCHAR LogNames::VIEWER_LOG_FILE_STUB_NAME[] = _T("tvnviewer");
const TCHAR LogNames::LOG_DIR_NAME[] = _T("TightVNC");
const TCHAR LogNames::LOG_PIPE_PUBLIC_NAME[] = _T("TVN_log_pipe_public_name");
const TCHAR LogNames::WIN_EVENT_PROVIDER_NAME[] = _T("tvnserver");

const TCHAR ClipboardNames::CLIPBOARD_WIN_CLASS_NAME[] = 
  _T("Tvnserver.Clipboard.MessageWindow");


const TCHAR ServiceNames::TVNCONTROL_START_REGISTRY_ENTRY_NAME[] =
  _T("tvncontrol");
const TCHAR ServiceNames::SERVICE_NAME[] = _T("tvnserver");
const TCHAR ServiceNames::SERVICE_NAME_TO_DISPLAY[] = _T("TightVNC Server");

const TCHAR ServerApplicationNames::SERVER_INSTANCE_MUTEX_NAME[] =
  _T("tvnserverApplication");
const TCHAR ServerApplicationNames::CONTROL_APP_INSTANCE_MUTEX_NAME[] =
  _T("tvnserverServiceControlSlave");
const TCHAR ServerApplicationNames::FOR_SERVICE_CONTROL_APP_PIPE_NAME[] =
  _T("TightVNC_Service_Control");
const TCHAR ServerApplicationNames::FOR_APP_CONTROL_APP_SERVICE_PIPE_NAME[] =
  _T("TightVNC_Application_Control");

const TCHAR HookDefinitions::HOOK_LOADER_WINDOW_CLASS[] =
  _T("TvnHookLoaderWindowClass");
const TCHAR HookDefinitions::HOOK_TARGET_WIN_CLASS_NAME[] =
  _T("HookTargetWinClassName");
const TCHAR HookDefinitions::HOOK_LOADER_NAME[] = _T("hookldr.exe");
const UINT HookDefinitions::LOADER_CLOSE_CODE =
  RegisterWindowMessage(_T("TVN.HOOK.LOADER.CLOSE.CODE"));
const UINT HookDefinitions::SPEC_IPC_CODE =
  RegisterWindowMessage(_T("TVN.HOOK.MESSAGE.CODE"));

const TCHAR DefaultNames::DEFAULT_COMPUTER_NAME[] = _T("TightVNC Server");


const char HttpStrings::HTTP_INDEX_PAGE_FORMAT[] =
"<HTML>\n"
"  <HEAD><TITLE>TightVNC desktop [%.256s]</TITLE></HEAD>\n"
"  <BODY>\n"
"    <APPLET ARCHIVE=\"tightvnc-jviewer.jar\""
" CODE=\"com.glavsoft.viewer.Viewer\" WIDTH=1 HEIGHT=1>\n"
"      <PARAM NAME=\"PORT\" VALUE=\"%d\">\n"
"      <PARAM NAME=\"OpenNewWindow\" VALUE=\"YES\">\n"
"%.1024s"
"    </APPLET><BR>\n"
"    <A HREF=\"http://www.tightvnc.com/\">www.TightVNC.com</A>\n"
"  </BODY>\n"
"</HTML>\n";

const char HttpStrings::HTTP_MSG_BADPARAMS[] =
"<HTML>\n"
"  <HEAD><TITLE>TightVNC desktop</TITLE></HEAD>\n"
"  <BODY>\n"
"    <H1>Bad Parameters</H1>\n"
"    The sequence of applet parameters specified within the URL is invalid.\n"
"  </BODY>\n"
"</HTML>\n";
