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

#include "ControlApplication.h"
#include "ControlTrayIcon.h"
#include "ControlTrayIcon.h"
#include "ControlPipeName.h"
#include "ControlCommand.h"
#include "ReloadConfigCommand.h"
#include "DisconnectAllCommand.h"
#include "SharePrimaryCommand.h"
#include "ShareDisplayCommand.h"
#include "ShareWindowCommand.h"
#include "ShareRectCommand.h"
#include "ShareFullCommand.h"
#include "ShareAppCommand.h"
#include "ControlAuth.h"
#include "ConnectCommand.h"
#include "DispatchCommand.h"
#include "ShutdownCommand.h"

#include "util/VncPassCrypt.h"
#include "util/winhdr.h"
#include "util/StringTable.h"
#include "tvnserver-app/NamingDefs.h"

#include "tvncontrol-app/ControlCommandLine.h"
#include "tvnserver-app/TvnServerHelp.h"

#include "win-system/Environment.h"
#include "win-system/Shell.h"
#include "win-system/Process.h"
#include "win-system/WinCommandLineArgs.h"

#include "thread/ZombieKiller.h"
#include "thread/GlobalMutex.h"

#include "gui/CommonControlsEx.h"

#include "network/socket/WindowsSocket.h"

#include "tvnserver/resource.h"

#include "wsconfig-lib/ConfigDialog.h"
#include "util/AnsiStringStorage.h"
#include "tvnserver-app/NamingDefs.h"
#include "SetPasswordsDialog.h"
#include <algorithm>

ControlApplication::ControlApplication(HINSTANCE hinst,
                                       const TCHAR *windowClassName,
                                       const TCHAR *commandLine)
 : WindowsApplication(hinst, windowClassName),
   m_serverControl(0),
   m_gate(0),
   m_transport(0),
   m_trayIcon(0),
   m_slaveModeEnabled(false),
   m_configurator(false),
   m_log(0)
{
  m_commandLine.setString(commandLine);

  CommonControlsEx::init();

  WindowsSocket::startup(2, 1);
}

ControlApplication::~ControlApplication()
{
  try {
    WindowsSocket::cleanup();
  } catch (...) { }

  if (m_serverControl != 0) {
    delete m_serverControl;
  }
  if (m_gate != 0) {
    delete m_gate;
  }
  if (m_transport != 0) {
    delete m_transport;
  }
}

int ControlApplication::run()
{
  ControlCommandLine cmdLineParser;

  // Check command line for valid.
  try {
    WinCommandLineArgs cmdArgs(m_commandLine.getString());
    cmdLineParser.parse(&cmdArgs);
  } catch (CommandLineFormatException &) {
    TvnServerHelp::showUsage();
    return 1;
  }

  // Run configuration dialog and exit.
  if (cmdLineParser.hasConfigAppFlag() || cmdLineParser.hasConfigServiceFlag()) {
    return runConfigurator(cmdLineParser.hasConfigServiceFlag(), cmdLineParser.hasDontElevateFlag());
  }

  if (cmdLineParser.hasCheckServicePasswords()) {
    return checkServicePasswords(cmdLineParser.hasDontElevateFlag());
  }

  // Change passwords and exit.
  if (cmdLineParser.hasSetControlPasswordFlag() || cmdLineParser.hasSetVncPasswordFlag()) {
    Configurator::getInstance()->setServiceFlag(true);
    Configurator::getInstance()->load();
    ServerConfig *config = Configurator::getInstance()->getServerConfig();
    UINT8 cryptedPass[8];
    if (cmdLineParser.hasSetControlPasswordFlag()) {
      getCryptedPassword(cryptedPass, cmdLineParser.getControlPassword());
      config->setControlPassword((const unsigned char *)cryptedPass);
      config->useControlAuth(true);
    } else {
      getCryptedPassword(cryptedPass, cmdLineParser.getPrimaryVncPassword());
      config->setPrimaryPassword((const unsigned char *)cryptedPass);
      config->useAuthentication(true);
    }
    Configurator::getInstance()->save();
    return 0;
  }

  int retCode = 0;

  // If we are in the "-controlservice -slave" mode, make sure there are no
  // other "service slaves" in this session, exit if there is one already.

  GlobalMutex *appGlobalMutex = 0;

  if (cmdLineParser.hasControlServiceFlag() && cmdLineParser.isSlave()) {
    try {
      appGlobalMutex = new GlobalMutex(
        ServerApplicationNames::CONTROL_APP_INSTANCE_MUTEX_NAME, false, true);
    } catch (...) {
      return 1;
    }
  }

  ZombieKiller zombieKiller;

  // Connect to server.
  try {
    connect(cmdLineParser.hasControlServiceFlag(), cmdLineParser.isSlave());
  } catch (Exception &) {
    if (!cmdLineParser.isSlave() && !cmdLineParser.hasCheckServicePasswords()) {
      const TCHAR *msg = StringTable::getString(IDS_FAILED_TO_CONNECT_TO_CONTROL_SERVER);
      const TCHAR *caption = StringTable::getString(IDS_MBC_TVNCONTROL);
      MessageBox(0, msg, caption, MB_OK | MB_ICONERROR);
    }
    return 1;
  }

  // Execute command (if specified) and exit.
  if (cmdLineParser.isCommandSpecified()) {
    Command *command = 0;

    StringStorage passwordFile;
    cmdLineParser.getPasswordFile(&passwordFile);
    m_serverControl->setPasswordProperties(passwordFile.getString(), true,
                                           cmdLineParser.hasControlServiceFlag());

    if (cmdLineParser.hasKillAllFlag()) {
      command = new DisconnectAllCommand(m_serverControl);
    } else if (cmdLineParser.hasReloadFlag()) {
      command = new ReloadConfigCommand(m_serverControl);
    } else if (cmdLineParser.hasConnectFlag()) {
      StringStorage hostName;
      cmdLineParser.getConnectHostName(&hostName);
      command = new ConnectCommand(m_serverControl, hostName.getString());
    } else if (cmdLineParser.hasDispatchFlag()) {
      StringStorage dispatcherSpec;
      cmdLineParser.getDispatcherSpec(&dispatcherSpec);
      command = new DispatchCommand(m_serverControl, dispatcherSpec.getString());
    } else if (cmdLineParser.hasShutdownFlag()) {
      command = new ShutdownCommand(m_serverControl);
    } else if (cmdLineParser.hasSharePrimaryFlag()) {
      command = new SharePrimaryCommand(m_serverControl);
    } else if (cmdLineParser.hasShareDisplay()) {
      unsigned char displayNumber = cmdLineParser.getShareDisplayNumber();
      command = new ShareDisplayCommand(m_serverControl, displayNumber);
    } else if (cmdLineParser.hasShareWindow()) {
      StringStorage shareWindowName;
      cmdLineParser.getShareWindowName(&shareWindowName);
      command = new ShareWindowCommand(m_serverControl, &shareWindowName);
    } else if (cmdLineParser.hasShareRect()) {
      Rect shareRect = cmdLineParser.getShareRect();
      command = new ShareRectCommand(m_serverControl, &shareRect);
    } else if (cmdLineParser.hasShareFull()) {
      command = new ShareFullCommand(m_serverControl);
    } else if (cmdLineParser.hasShareApp()) {
      command = new ShareAppCommand(m_serverControl, cmdLineParser.getSharedAppProcessId());
    }

    retCode = runControlCommand(command);

    if (command != 0) {
      delete command;
    }
  } else {
    bool showIcon = true;

    if (cmdLineParser.isSlave()) {
      m_slaveModeEnabled = true;
      try {
        try {
          showIcon = m_serverControl->getShowTrayIconFlag();
        } catch (RemoteException &remEx) {
          notifyServerSideException(remEx.getMessage());
        }
        try {
          m_serverControl->updateTvnControlProcessId(GetCurrentProcessId());
        } catch (RemoteException &remEx) {
          notifyServerSideException(remEx.getMessage());
        }
      } catch (IOException &) {
        notifyConnectionLost();
        return 1;
      } catch (Exception &) {
        _ASSERT(FALSE);
      }
    }

    retCode = runControlInterface(showIcon);
  }

  if (appGlobalMutex != 0) {
    delete appGlobalMutex;
  }

  return retCode;
}

void ControlApplication::connect(bool controlService, bool slave)
{
  // Determine the name of pipe to connect to.
  StringStorage pipeName;
  ControlPipeName::createPipeName(controlService, &pipeName, &m_log);

  int numTriesRemaining = slave ? 10 : 1;
  int msDelayBetweenTries = 2000;

  while (numTriesRemaining-- > 0) {
    try {
      m_transport = TransportFactory::createPipeClientTransport(pipeName.getString());
      break;
    } catch (Exception &) {
      if (numTriesRemaining <= 0) {
        throw;
      }
    }
    Sleep(msDelayBetweenTries);
  }

  // We can get here only on successful connection.
  m_gate = new ControlGate(m_transport->getIOStream());
  m_serverControl = new ControlProxy(m_gate);
}

void ControlApplication::notifyServerSideException(const TCHAR *reason)
{
  StringStorage message;

  message.format(StringTable::getString(IDS_CONTROL_SERVER_RAISE_EXCEPTION), reason);

  MessageBox(0, message.getString(), StringTable::getString(IDS_MBC_TVNSERVER), MB_OK | MB_ICONERROR);
}

void ControlApplication::notifyConnectionLost()
{
  MessageBox(0,
             StringTable::getString(IDS_CONTROL_CONNECTION_LOST),
             StringTable::getString(IDS_MBC_TVNCONTROL),
             MB_OK | MB_ICONEXCLAMATION);
}

void ControlApplication::execute()
{
  try {
    while (!isTerminating()) {
      Thread::sleep(500);
      // If we need to show or hide icon.
      bool showIcon = m_serverControl->getShowTrayIconFlag() || !m_slaveModeEnabled;

      // Check if we need to show icon.
      if (showIcon && !m_trayIcon->isVisible()) {
        m_trayIcon->show();
      }
      // Check if we need to hide icon.
      if (m_trayIcon != 0 && !showIcon) {
        m_trayIcon->hide();
      }
      // Update tray icon status if icon is set.
      if (m_trayIcon->isVisible()) {
        m_trayIcon->syncStatusWithServer();
      }
    }
  } catch (...) {
    m_trayIcon->terminate();
    m_trayIcon->waitForTermination();
    shutdown();
  }
}

int ControlApplication::runControlInterface(bool showIcon)
{
  m_trayIcon = new ControlTrayIcon(m_serverControl, this, this, showIcon);

  resume();

  int ret = WindowsApplication::run();

  terminate();
  wait();

  delete m_trayIcon;

  return ret;
}

int ControlApplication::runControlCommand(Command *command)
{
  ControlCommand ctrlCmd(command);

  ctrlCmd.execute();

  int errorCode = ctrlCmd.executionResultOk() ? 0 : 1;
  return errorCode;
}

int ControlApplication::runConfigurator(bool configService, bool isRunAsRequested)
{
  // If not enough rights to configurate service, then restart application requesting
  // admin access rights.
  if (configService && (IsUserAnAdmin() == FALSE)) {
    // If admin rights already requested and application still don't have them,
    // then show error message and exit.
    if (isRunAsRequested) {
      MessageBox(0,
        StringTable::getString(IDS_ADMIN_RIGHTS_NEEDED),
        StringTable::getString(IDS_MBC_TVNCONTROL),
        MB_OK | MB_ICONERROR);
      return 0;
    }
    // Path to tvnserver binary.
    StringStorage pathToBinary;
    // Command line for child process.
    StringStorage childCommandLine;

    // Get path to tvnserver binary.
    Environment::getCurrentModulePath(&pathToBinary);
    // Set -dontelevate flag to tvncontrol know that admin rights already requested.
    childCommandLine.format(_T("%s -dontelevate"), m_commandLine.getString());

    // Start child.
    try {
      Shell::runAsAdmin(pathToBinary.getString(), childCommandLine.getString());
    } catch (SystemException &sysEx) {
      if (sysEx.getErrorCode() != ERROR_CANCELLED) {
        MessageBox(0,
          sysEx.getMessage(),
          StringTable::getString(IDS_MBC_TVNCONTROL),
          MB_OK | MB_ICONERROR);
      }
      return 1;
    } // try / catch.
    return 0;
  }

  Configurator *configurator = Configurator::getInstance();

  configurator->setServiceFlag(configService);
  configurator->load();

  ConfigDialog confDialog(configService, 0);

  return confDialog.showModal();
}

void ControlApplication::getCryptedPassword(UINT8 cryptedPass[8], const TCHAR *plainTextPassString)
{
  // Get a copy of the password truncated at 8 characters.
  StringStorage plainTextPass(plainTextPassString);
  plainTextPass.getSubstring(&plainTextPass, 0, 7);
  // Convert from TCHAR[] to char[].
  // FIXME: Check exception catching.
  AnsiStringStorage ansiPass(&plainTextPass);

  // Convert to a byte array.
  UINT8 byteArray[8] = {0, 0, 0, 0, 0, 0, 0, 0};
  size_t len = std::min(ansiPass.getLength(), (size_t)8);
  memcpy(byteArray, ansiPass.getString(), len);

  // Encrypt with a fixed key.
  VncPassCrypt::getEncryptedPass(cryptedPass, byteArray);
}

int ControlApplication::checkServicePasswords(bool isRunAsRequested)
{
  // FIXME: code duplication.
  if (IsUserAnAdmin() == FALSE) {
    // If admin rights already requested and application still don't have them,
    // then show error message and exit.
    if (isRunAsRequested) {
      MessageBox(0,
        StringTable::getString(IDS_ADMIN_RIGHTS_NEEDED),
        StringTable::getString(IDS_MBC_TVNCONTROL),
        MB_OK | MB_ICONERROR);
      return 1;
    }
    // Path to tvnserver binary.
    StringStorage pathToBinary;
    // Command line for child process.
    StringStorage childCommandLine;

    // Get path to tvnserver binary.
    Environment::getCurrentModulePath(&pathToBinary);
    // Set -dontelevate flag to tvncontrol know that admin rights already requested.
    childCommandLine.format(_T("%s -dontelevate"), m_commandLine.getString());

    // Start child.
    try {
      Shell::runAsAdmin(pathToBinary.getString(), childCommandLine.getString());
      return 0;
    } catch (SystemException &sysEx) {
      if (sysEx.getErrorCode() != ERROR_CANCELLED) {
        MessageBox(0,
          sysEx.getMessage(),
          StringTable::getString(IDS_MBC_TVNCONTROL),
          MB_OK | MB_ICONERROR);
      }
      return 1;
    } // try / catch.
    return 0;
  }
  checkServicePasswords();
  return 0;
}

void ControlApplication::checkServicePasswords()
{
  Configurator::getInstance()->setServiceFlag(true);
  Configurator::getInstance()->load();
  ServerConfig *config = Configurator::getInstance()->getServerConfig();

  bool askToChangeRfbAuth = !config->isUsingAuthentication() || !config->hasPrimaryPassword();
  bool askToChangeAdmAuth = !config->isControlAuthEnabled() || !config->hasControlPassword();
  SetPasswordsDialog dialog(askToChangeRfbAuth, askToChangeAdmAuth);
  if (dialog.showModal() == IDOK) {
    UINT8 cryptedPass[8];
    bool useRfbAuth = dialog.getUseRfbPass();
    bool dontUseRfbAuth = dialog.getRfbPassForClear();
    // Note: The state !useRfbAuth && !dontUseRfbAuth is valid and means "do not change
    // the auth settings".
    if (useRfbAuth) {
      StringStorage pass;
      dialog.getRfbPass(&pass);
      getCryptedPassword(cryptedPass, pass.getString());
      config->setPrimaryPassword(cryptedPass);
      config->useAuthentication(true);
    } else if (dontUseRfbAuth) {
      config->deletePrimaryPassword();
      config->deleteReadOnlyPassword();
      config->useAuthentication(false);
    }
    bool useAdmAuth = dialog.getUseAdmPass();
    bool dontUseAdmAuth = dialog.getAdmPassForClear();
    if (useAdmAuth) {
      StringStorage pass;
      dialog.getAdmPass(&pass);
      getCryptedPassword(cryptedPass, pass.getString());
      config->setControlPassword(cryptedPass);
      config->useControlAuth(true);
    } else if (dontUseAdmAuth) {
      config->deleteControlPassword();
      config->useControlAuth(false);
    }
    Configurator::getInstance()->save();
    reloadConfig();
  }
}

void ControlApplication::reloadConfig()
{
  StringStorage pathToBinary;
  try {
    // Get path to tvnserver binary.
    Environment::getCurrentModulePath(&pathToBinary);
    Process processToReloadConfig(pathToBinary.getString(), _T("-controlservice -reload"));
    processToReloadConfig.start();
  } catch (Exception &e) {
    StringStorage errMess;
    errMess.format(StringTable::getString(IDS_FAILED_TO_RELOAD_SERVICE_ON_CHECK_PASS), e.getMessage());
    MessageBox(0,
      errMess.getString(),
      StringTable::getString(IDS_MBC_TVNCONTROL),
      MB_OK | MB_ICONERROR);
  }
}
