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

#ifndef _VIEWER_CMD_LINE_H_
#define _VIEWER_CMD_LINE_H_

#include "win-system/WinProcessCommandLine.h"
#include "client-config-lib/ViewerConfig.h"
#include "util/CommandLineFormatException.h"
#include "util/CommandLineFormatHelp.h"
#include "HelpDialog.h"
#include "ConnectionData.h"

class ViewerCmdLine {
public:
  ViewerCmdLine(ConnectionData *conData,
                ConnectionConfig *conConf,
                ViewerConfig *config,
                bool *isListening);

  // this function parse the command line
  void parse() throw(CommandLineFormatException, CommandLineFormatHelp);

  // this function creates the modal dialog with text how to use it
  void onHelp();

protected:
  void parseHostArg();
  bool parseHostOptions();
  bool isPresent(const TCHAR *keyName);

  static const TCHAR HELP[];
  static const TCHAR HELP_SHORT[];
  static const TCHAR HELP_ARG[];
  static const TCHAR HELP_ARG_SHORT[];
  static const TCHAR HELP_ARG_QUESTION[];
  static const TCHAR OPTIONS_FILE[];
  static const TCHAR LISTEN[];
  static const TCHAR HOST[];
  static const TCHAR PORT[];
  static const TCHAR PASSWORD[];
  static const TCHAR DISPATCH_ID[];
  static const TCHAR SHOW_CONTROLS[];
  static const TCHAR VIEW_ONLY[];
  static const TCHAR USE_CLIPBOARD[];
  static const TCHAR SCALE[];
  static const TCHAR FULL_SCREEN[];
  static const TCHAR WARN_FULL_SCREEN[];
  static const TCHAR ENCODING[];
  static const TCHAR COPY_RECT[];
  static const TCHAR MOUSE_CURSOR[];
  static const TCHAR MOUSE_LOCAL[];
  static const TCHAR MOUSE_SWAP[];
  static const TCHAR JPEG_IMAGE_QUALITY[];
  static const TCHAR COMPRESSION_LEVEL[];
  static const TCHAR LOG_PATH[];


  static const TCHAR YES[];
  static const TCHAR NO[];
  static const TCHAR AUTO[];
  static const TCHAR LOCAL[];
  static const TCHAR SMALLDOT[];
  static const TCHAR NORMAL[];
  static const TCHAR HEXTILE[];
  static const TCHAR TIGHT[];
  static const TCHAR RRE[];
  static const TCHAR ZRLE[];

  WinProcessCommandLine m_wpcl;
  ConnectionData *m_conData;
  ConnectionConfig *m_conConf;
  ViewerConfig *m_config;
  bool *m_isListening;

  std::map<StringStorage, StringStorage> m_options;

private:
  struct CmdLineOption
  {
    const TCHAR *keyName;
  };

  bool processCmdLine(const CmdLineOption * cmdLines, 
                      size_t lenCmdLineOption);

  bool isHelpPresent();
  void parseOptionsFile();
  void parsePassword();
  void parseDispatchId();
  void parseEncoding();
  void parseMouseShape();
  void parseMouseCursor();
  void parseScale();
  void parseFullScreen();
  void parseCompressionLevel();
  void parseWarnFullScr();
  void parseMouseSwap();
  void parseUseClipboard();
  void parseShowControl();
  void parseCopyRect();
  void parseViewOnly();
  void parseJpegImageQuality();
  bool parseHost();
  void parseLogPath();
};

#endif
