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

#include "ViewerCmdLine.h"
#include "config-lib/IniFileSettingsManager.h"
#include "win-system/SystemException.h"

const TCHAR ViewerCmdLine::HELP[] = _T("help");
const TCHAR ViewerCmdLine::HELP_SHORT[] = _T("h");
const TCHAR ViewerCmdLine::HELP_ARG[] = _T("/help");
const TCHAR ViewerCmdLine::HELP_ARG_SHORT[] = _T("/h");
const TCHAR ViewerCmdLine::HELP_ARG_QUESTION[] = _T("/?");

const TCHAR ViewerCmdLine::OPTIONS_FILE[] = _T("optionsfile");
const TCHAR ViewerCmdLine::LISTEN[] = _T("listen");
const TCHAR ViewerCmdLine::HOST[] = _T("host");
const TCHAR ViewerCmdLine::PORT[] = _T("port");
const TCHAR ViewerCmdLine::PASSWORD[] = _T("password");
const TCHAR ViewerCmdLine::SHOW_CONTROLS[] = _T("showcontrols");
const TCHAR ViewerCmdLine::VIEW_ONLY[] = _T("viewonly");
const TCHAR ViewerCmdLine::USE_CLIPBOARD[] = _T("useclipboard");
const TCHAR ViewerCmdLine::SCALE[] = _T("scale");
const TCHAR ViewerCmdLine::FULL_SCREEN[] = _T("fullscreen");
const TCHAR ViewerCmdLine::WARN_FULL_SCREEN[] = _T("warnfullscr");
const TCHAR ViewerCmdLine::ENCODING[] = _T("encoding");
const TCHAR ViewerCmdLine::COPY_RECT[] = _T("copyrect");
const TCHAR ViewerCmdLine::MOUSE_CURSOR[] = _T("mousecursor");
const TCHAR ViewerCmdLine::MOUSE_LOCAL[] = _T("mouselocal");
const TCHAR ViewerCmdLine::MOUSE_SWAP[] = _T("mouseswap");
const TCHAR ViewerCmdLine::JPEG_IMAGE_QUALITY[] = _T("jpegimagequality");
const TCHAR ViewerCmdLine::COMPRESSION_LEVEL[] = _T("compressionlevel");
const TCHAR ViewerCmdLine::LOG_PATH[] = _T("logpath");


const TCHAR ViewerCmdLine::YES[] = _T("yes");
const TCHAR ViewerCmdLine::NO[] = _T("no");
const TCHAR ViewerCmdLine::AUTO[] = _T("auto");
const TCHAR ViewerCmdLine::LOCAL[] = _T("local");
const TCHAR ViewerCmdLine::SMALLDOT[] = _T("smalldot");
const TCHAR ViewerCmdLine::NORMAL[] = _T("normal");
const TCHAR ViewerCmdLine::HEXTILE[] = _T("hextile");
const TCHAR ViewerCmdLine::TIGHT[] = _T("tight");
const TCHAR ViewerCmdLine::RRE[] = _T("rre");
const TCHAR ViewerCmdLine::ZRLE[] = _T("zrle");

ViewerCmdLine::ViewerCmdLine(ConnectionData *conData,
                             ConnectionConfig *conConf,
                             ViewerConfig *config,
                             bool *isListening)
: m_conData(conData),
  m_conConf(conConf),
  m_config(config),
  m_isListening(isListening)
{
}

bool ViewerCmdLine::processCmdLine(const CmdLineOption *cmdLines, size_t lenCmdLineOption)
{
  if (m_wpcl.getOptionsCount()) {
    int countRecog = 0;

    for (size_t i = 0; i < lenCmdLineOption; i++) {
       StringStorage strOut;

       for (size_t j = 0; j < m_wpcl.getOptionsCount(); j++) {
         m_wpcl.getOption(j , &strOut);

         if (strOut == cmdLines[i].keyName) {
           countRecog++;
         }
       }
       if (m_wpcl.findOptionValue(cmdLines[i].keyName, &strOut)) {
         m_options[cmdLines[i].keyName] = strOut; 
       }
    }
    if (countRecog != m_wpcl.getOptionsCount()) {
      return false;
    }
  }
  return true;
}

void ViewerCmdLine::parse()
{
  const CmdLineOption options[] = {
    HELP,
    HELP_SHORT,
    OPTIONS_FILE,
    LISTEN,
    HOST,
    PORT,
    PASSWORD,
    SHOW_CONTROLS,
    VIEW_ONLY,
    USE_CLIPBOARD,
    SCALE,
    FULL_SCREEN,
    WARN_FULL_SCREEN,
    ENCODING,
    COPY_RECT,
    MOUSE_CURSOR,
    MOUSE_LOCAL,
    MOUSE_SWAP,
    JPEG_IMAGE_QUALITY,
    COMPRESSION_LEVEL,
    LOG_PATH
  };

  if (!processCmdLine(&options[0], sizeof(options) / sizeof(CmdLineOption))) {
    throw CommandLineFormatException(StringTable::getString(IDS_ERROR_COMMAND_LINE));
  }

  // If options "help" is present, then show "Help dialog" and exit.
  if (isHelpPresent()) {
    throw CommandLineFormatHelp();
  }


  if (m_wpcl.getArgumentsCount() > 2) {
    throw CommandLineFormatException(StringTable::getString(IDS_ERROR_COMMAND_LINE));
  }

  if (m_wpcl.getArgumentsCount() > 1) {
    if (isPresent(ViewerCmdLine::HOST)) {
      throw CommandLineFormatException(StringTable::getString(IDS_ERROR_COMMAND_LINE));
    }
  }

  if (isPresent(ViewerCmdLine::OPTIONS_FILE)) {
    parseOptionsFile();
  } else if (isPresent(ViewerCmdLine::LISTEN)) {
      *m_isListening = true;
  } else if (isPresent(ViewerCmdLine::LOG_PATH)) {
    parseLogPath();
  } else if (!parseHost()) {
      throw CommandLineFormatException(StringTable::getString(IDS_ERROR_COMMAND_LINE));
  }
  parsePassword();
  parseEncoding();
  parseMouseShape();
  parseMouseCursor();
  parseScale();
  parseFullScreen();
  parseCompressionLevel();
  parseWarnFullScr();
  parseMouseSwap();
  parseUseClipboard();
  parseShowControl();
  parseCopyRect();
  parseViewOnly();
  parseJpegImageQuality();
}

void ViewerCmdLine::onHelp()
{
  HelpDialog hlpdlg;

  hlpdlg.showModal();
}

bool ViewerCmdLine::isHelpPresent()
{
  for (size_t i = 0; i < m_wpcl.getArgumentsCount(); i++) {
    StringStorage argument;
    if (m_wpcl.getArgument(i, &argument)) {
      if (argument.isEqualTo(HELP_ARG))
        return true;
      if (argument.isEqualTo(HELP_ARG_SHORT))
        return true;
      if (argument.isEqualTo(HELP_ARG_QUESTION))
        return true;
    }
  }

  if (isPresent(HELP))
    return true;
  if (isPresent(HELP_SHORT))
    return true;
  return false;
}

bool ViewerCmdLine::isPresent(const TCHAR *keyName)
{
  return m_options.find(keyName) != m_options.end();
}

void ViewerCmdLine::parseOptionsFile()
{
  StringStorage pathToIniFile = m_options[OPTIONS_FILE];
  if (pathToIniFile.findChar(_T('\\')) == -1) {
    StringStorage newPathToIniFile;
    newPathToIniFile.format(_T(".\\%s"), pathToIniFile.getString());
    pathToIniFile = newPathToIniFile;
  }
  IniFileSettingsManager sm(pathToIniFile.getString());
  sm.setApplicationName(_T("connection"));

  StringStorage host;
  if (!sm.getString(_T("host"), &host)) {
    throw CommandLineFormatException(StringTable::getString(IDS_ERROR_PARSE_OPTIONS_FILE));
  }
  StringStorage port;
  if (sm.getString(_T("port"), &port)) {
    StringStorage hostString;
    hostString.format(_T("%s::%s"), host.getString(), port.getString());
    m_conData->setHost(&hostString);
  } else {
    m_conData->setHost(&host);
  }

  StringStorage password;
  sm.getString(_T("password"), &password);
  if (!password.isEmpty()) {
    m_conData->setCryptedPassword(&password);
  } else {
    parsePassword();
  }
  
  sm.setApplicationName(_T("options"));
  m_conConf->loadFromStorage(&sm);
}

void ViewerCmdLine::parsePassword()
{
  if (isPresent(PASSWORD)) {
    m_conData->setPlainPassword(&m_options[PASSWORD]);
  }
}

void ViewerCmdLine::parseEncoding()
{
  if (isPresent(ENCODING)) {
    int iEncoding = EncodingDefs::RAW;

    if (m_options[ENCODING] == HEXTILE) {
      iEncoding = EncodingDefs::HEXTILE;
    }
    if (m_options[ENCODING] == TIGHT) {
      iEncoding = EncodingDefs::TIGHT;
    }
    if (m_options[ENCODING] == RRE) {
      iEncoding = EncodingDefs::RRE;
    }
    if (m_options[ENCODING] == ZRLE) {
      iEncoding = EncodingDefs::ZRLE;
    }
    m_conConf->setPreferredEncoding(iEncoding);
  }
}

void ViewerCmdLine::parseMouseShape() 
{
  if (isPresent(MOUSE_LOCAL)) {
    int localCursorShape = ConnectionConfig::DOT_CURSOR;

    if (m_options[MOUSE_LOCAL] == NO) {
      localCursorShape = ConnectionConfig::NO_CURSOR;
    }
    if (m_options[MOUSE_LOCAL] == SMALLDOT) {
      localCursorShape = ConnectionConfig::SMALL_CURSOR;
    }
    if (m_options[MOUSE_LOCAL] == NORMAL) {
      localCursorShape = ConnectionConfig::NORMAL_CURSOR;
    }
    m_conConf->setLocalCursorShape(localCursorShape);
  }
}

void ViewerCmdLine::parseMouseCursor()
{
  if (isPresent(MOUSE_CURSOR)) {
    m_conConf->requestShapeUpdates(false);
    m_conConf->ignoreShapeUpdates(false);
    if (m_options[MOUSE_CURSOR] == NO) {
      m_conConf->requestShapeUpdates(true);
      m_conConf->ignoreShapeUpdates(true);
    } else {
      if (m_options[MOUSE_CURSOR] == LOCAL) {
        m_conConf->requestShapeUpdates(true);
      }
    }
  }
}

void ViewerCmdLine::parseScale()
{
  if (isPresent(SCALE)) {
    if (m_options[SCALE] == AUTO) {
      m_conConf->fitWindow(true);
    } else {
      int scale = _ttoi(m_options[SCALE].getString());

      if (scale < 1) {
        scale = 1;
      }
      if (scale > 400) {
        scale = 400;
      }
      m_conConf->setScale(scale, 100);
      m_conConf->fitWindow(false);
    }
  }
}

void ViewerCmdLine::parseFullScreen()
{
  if (isPresent(FULL_SCREEN)) {
    bool isFullScreen = false;

    if (m_options[FULL_SCREEN] == YES) {
      isFullScreen = true;
    }
    m_conConf->enableFullscreen(isFullScreen);
  }
}

void ViewerCmdLine::parseWarnFullScr()
{
  if (isPresent(WARN_FULL_SCREEN)) {
    bool isWarn = false;

    if (m_options[WARN_FULL_SCREEN] == YES) {
      isWarn = true;
    }
    m_config->promptOnFullscreen(isWarn);
  }
}

void ViewerCmdLine::parseMouseSwap()
{
  if (isPresent(MOUSE_SWAP)) {
    bool isMouseSwap = false;

    if (m_options[MOUSE_SWAP] == YES) {
      isMouseSwap = true;
    }
    m_conConf->swapMouse(isMouseSwap);
  }
}

void ViewerCmdLine::parseUseClipboard()
{
  if (isPresent(USE_CLIPBOARD)) {
    bool isUseClipboard = false;

    if (m_options[USE_CLIPBOARD] == YES) {
      isUseClipboard = true;
    }
    m_conConf->enableClipboard(isUseClipboard);
  }
}

void ViewerCmdLine::parseShowControl()
{
  if (isPresent(SHOW_CONTROLS)) {
    bool isShowControl = false;

    if (m_options[SHOW_CONTROLS] == YES) {
      isShowControl = true;
    }
    m_config->showToolbar(isShowControl);
  }
}

void ViewerCmdLine::parseCopyRect()
{
  if (isPresent(COPY_RECT)) {
    bool isCopyRect = false;

    if (m_options[COPY_RECT] == YES) {
      isCopyRect = true;
    }
    m_conConf->allowCopyRect(isCopyRect);
  }
}

void ViewerCmdLine::parseViewOnly()
{
  if (isPresent(VIEW_ONLY)) {
    bool isViewOnly = false;

    if (m_options[VIEW_ONLY] == ViewerCmdLine::YES) {
      isViewOnly = true;
    }
    m_conConf->setViewOnly(isViewOnly);
  }
}

void ViewerCmdLine::parseJpegImageQuality()
{
  if (isPresent(JPEG_IMAGE_QUALITY)) {
    int iJpegQuality = _ttoi(m_options[JPEG_IMAGE_QUALITY].getString());
    m_conConf->setJpegCompressionLevel(iJpegQuality);
  }
}

void ViewerCmdLine::parseCompressionLevel()
{
  if (isPresent(COMPRESSION_LEVEL)) {
    int iCompLvl = _ttoi(m_options[COMPRESSION_LEVEL].getString());
    m_conConf->setCustomCompressionLevel(iCompLvl);
  }
}

void ViewerCmdLine::parseHostArg()
{
  StringStorage host;
  m_wpcl.getArgument(1, &host);

  if (host.findChar(_T(':')) != -1) {
    m_conData->setHost(&host);
  } else {
   if (isPresent(PORT)) {
     StringStorage hostPort;

     hostPort.format(_T("%s::%s"), host.getString(),
                                  m_options[PORT].getString());
     m_conData->setHost(&hostPort);
   }
   m_conData->setHost(&host);
  }
}

bool ViewerCmdLine::parseHostOptions()
{
  if (!isPresent(HOST)) {
    if (m_wpcl.getOptionsCount()) {
      return false;
    }
    return true;
  }
  StringStorage strHost, strPort;

  if (!isPresent(PORT)) {
    strHost = m_options[HOST];
  } else {
    strHost.format(_T("%s::%s"), m_options[HOST].getString(),
                                m_options[PORT].getString());
  }
  m_conData->setHost(&strHost);
  return true;
}

bool ViewerCmdLine::parseHost()
{
  if (m_wpcl.getArgumentsCount() > 1) {
    parseHostArg();
  } else {
    return parseHostOptions();
  }
  return true;
}

void ViewerCmdLine::parseLogPath()
{
  if (isPresent(LOG_PATH)) {
    m_config->setLogDir(m_options[LOG_PATH]);
  }
}
