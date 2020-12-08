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

#include "DesktopClientImpl.h"
#include "server-config-lib/Configurator.h"
#include "desktop-ipc/UpdateHandlerClient.h"
#include "WindowsInputBlocker.h"
#include "desktop-ipc/UserInputClient.h"
#include "SasUserInput.h"
#include "WindowsUserInput.h"
#include "DesktopConfigLocal.h"

DesktopClientImpl::DesktopClientImpl(ClipboardListener *extClipListener,
                       UpdateSendingListener *extUpdSendingListener,
                       AbnormDeskTermListener *extDeskTermListener,
                       LogWriter *log)
: DesktopBaseImpl(extClipListener, extUpdSendingListener, extDeskTermListener, log),
  m_clToSrvChan(0),
  m_srvToClChan(0),
  m_clToSrvGate(0),
  m_srvToClGate(0),
  m_deskServWatcher(0),
  m_dispatcher(0),
  m_userInputClient(0),
  m_deskConf(0),
  m_gateKicker(0),
  m_log(log)
{
  m_log->info(_T("Creating DesktopClientImpl"));

  try {
    m_log->debug(_T("DesktopClientImpl: Try to initialize DesktopServerWatcher"));
    m_deskServWatcher = new DesktopServerWatcher(this, m_log);

    // Transport initialization
    m_log->debug(_T("DesktopClientImpl: Initializing ReconnectingChannel(s)..."));
    m_clToSrvChan = new ReconnectingChannel(60000, m_log);
    m_srvToClChan = new ReconnectingChannel(60000, m_log);

    // At this point the all DesktopServerWatcher's callback resources is initialized.
    m_log->debug(_T("DesktopClientImpl: Resuming DesktopServerWatcher"));
    m_deskServWatcher->resume();

    m_log->debug(_T("DesktopClientImpl: Creating BlockingGate wrappers for the ReconnectingChannel(s)"));
    m_clToSrvGate = new BlockingGate(m_clToSrvChan);
    m_srvToClGate = new BlockingGate(m_srvToClChan);

    m_log->debug(_T("DesktopClientImpl: Initializing DesktopSrvDispatcher"));
    m_dispatcher = new DesktopSrvDispatcher(m_srvToClGate, this, m_log);

    m_log->debug(_T("DesktopClientImpl: Initializing UpdateHandlerClient..."));
    m_updateHandler = new UpdateHandlerClient(m_clToSrvGate, m_dispatcher,
                                              this, m_log);

    m_log->debug(_T("DesktopClientImpl: Initializing UserInputClient..."));
    UserInputClient *userInputClient =
      new UserInputClient(m_clToSrvGate, m_dispatcher, this);
    m_userInputClient = userInputClient;
    m_log->debug(_T("DesktopClientImpl: Initializing SasUserInput..."));
    m_userInput = new SasUserInput(userInputClient, m_log);

    m_log->debug(_T("DesktopClientImpl: Initializing DesktopConfigClient..."));
    m_deskConf = new DesktopConfigClient(m_clToSrvGate);
    m_log->debug(_T("DesktopClientImpl: Initializing GateKicker..."));
    m_gateKicker = new GateKicker(m_clToSrvGate);
    // Start dispatcher after handler registrations
    m_log->debug(_T("DesktopClientImpl: Resuming DesktopSrvDispatcher"));
    m_dispatcher->resume();
    m_log->debug(_T("DesktopClientImpl: Calling onConfigReload(0)"));
    onConfigReload(0);

    m_log->debug(_T("DesktopClientImpl: Registering as a listener in the Configurator"));
    Configurator::getInstance()->addListener(this);
  } catch (Exception &ex) {
    m_log->error(_T("Exception during DesktopClientImpl creaion: %s"), ex.getMessage());
    freeResource();
    throw;
  }
  m_log->debug(_T("DesktopClientImpl: Resuming self thread"));
  resume();
}

DesktopClientImpl::~DesktopClientImpl()
{
  m_log->info(_T("Deleting DesktopClientImpl"));
  terminate();
  wait();
  freeResource();
  m_log->info(_T("DesktopClientImpl deleted"));
}

void DesktopClientImpl::freeResource()
{
  Configurator::getInstance()->removeListener(this);

  if (m_deskServWatcher) delete m_deskServWatcher;

  closeDesktopServerTransport();

  if (m_dispatcher) delete m_dispatcher;

  if (m_gateKicker) delete m_gateKicker;
  if (m_updateHandler) delete m_updateHandler;
  if (m_deskConf) delete m_deskConf;
  if (m_userInput) delete m_userInput;
  if (m_userInputClient) delete m_userInputClient;

  if (m_srvToClGate) delete m_srvToClGate;
  if (m_clToSrvGate) delete m_clToSrvGate;

  if (m_srvToClChan) delete m_srvToClChan;
  if (m_clToSrvChan) delete m_clToSrvChan;
}

void DesktopClientImpl::closeDesktopServerTransport()
{
  try {
    if (m_clToSrvChan) m_clToSrvChan->close();
  } catch (Exception &e) {
    m_log->error(_T("Cannot close client->server channel from Windesktop: %s"),
               e.getMessage());
  }
  try {
    if (m_srvToClChan) m_srvToClChan->close();
  } catch (Exception &e) {
    m_log->error(_T("Cannot close server->client channel from Windesktop: %s"),
               e.getMessage());
  }
}

void DesktopClientImpl::onAnObjectEvent()
{
  m_extDeskTermListener->onAbnormalDesktopTerminate();
  m_log->error(_T("Forced closing of pipe conections"));
  closeDesktopServerTransport();
}

void DesktopClientImpl::onReconnect(Channel *newChannelTo, Channel *newChannelFrom)
{
  BlockingGate gate(newChannelTo);
  if (m_deskConf) {
    m_log->info(_T("try update remote configuration from the ")
              _T("DesktopClientImpl::onReconnect() function"));
    m_deskConf->updateByNewSettings(&gate);
  }
  if (m_updateHandler) {
    m_log->info(_T("try update remote UpdateHandler from the ")
              _T("DesktopClientImpl::onReconnect() function"));
    m_updateHandler->sendInit(&gate);
  }
  if (m_userInput) {
    m_log->info(_T("try update remote UserInput from the ")
              _T("DesktopClientImpl::onReconnect() function"));
    m_userInput->sendInit(&gate);
  }

  m_clToSrvChan->replaceChannel(newChannelTo);
  m_srvToClChan->replaceChannel(newChannelFrom);
}

void DesktopClientImpl::onTerminate()
{
  m_newUpdateEvent.notify();
}

void DesktopClientImpl::execute()
{
  m_log->info(_T("DesktopClientImpl thread started"));

  while (!isTerminating()) {
    m_newUpdateEvent.waitForEvent();
    if (!isTerminating()) {
      sendUpdate();
    }
  }

  m_log->info(_T("DesktopClientImpl thread stopped"));
}

bool DesktopClientImpl::isRemoteInputTempBlocked()
{
  return !m_deskConf->isRemoteInputAllowed();
}

void DesktopClientImpl::applyNewConfiguration()
{
  m_log->info(_T("reload DesktopClientImpl configuration"));
  m_deskConf->updateByNewSettings(m_clToSrvGate);
}
