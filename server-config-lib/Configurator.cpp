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

#include "win-system/Environment.h"
#include "wsconfig-lib/TvnLogFilename.h"
#include "config-lib/RegistrySettingsManager.h"

#include "win-system/Registry.h"
#include "win-system/RegistryKey.h"

#include "Configurator.h"
#include "tvnserver-app/NamingDefs.h"

Configurator *Configurator::s_instance = NULL;
LocalMutex Configurator::m_instanceMutex;

Configurator::Configurator(bool isConfiguringService)
: m_isConfiguringService(isConfiguringService), m_isConfigLoadedPartly(false),
  m_isFirstLoad(true), m_regSA(0)
{
  AutoLock al(&m_instanceMutex);
  if (s_instance != 0) {
    throw Exception(_T("Configurator instance already exists"));
  }
  s_instance = this;
  try {
    m_regSA = new RegistrySecurityAttributes();
  } catch (...) {
    // TODO: Place exception handler here.
  }
}

Configurator::~Configurator()
{
  if (m_regSA != 0) delete m_regSA;
}

Configurator *Configurator::getInstance()
{
  AutoLock al(&m_instanceMutex);
  _ASSERT(s_instance != NULL);
  return s_instance;
}

void Configurator::setInstance(Configurator *conf)
{
  s_instance = conf;
}

void Configurator::notifyReload()
{
  AutoLock l(&m_listeners);

  for (size_t i = 0; i < m_listeners.size(); i++) {
    m_listeners.at(i)->onConfigReload(getServerConfig());
  }
}

bool Configurator::load()
{
  return load(m_isConfiguringService);
}

bool Configurator::save()
{
  return save(m_isConfiguringService);
}

bool Configurator::load(bool forService)
{
  bool isOk = false;

  HKEY rootKey = forService ? HKEY_LOCAL_MACHINE : HKEY_CURRENT_USER;

  SECURITY_ATTRIBUTES *sa = 0;
  if (forService && m_regSA != 0) {
    sa = m_regSA->getServiceSA();
  }
  RegistrySettingsManager sm(rootKey, RegistryPaths::SERVER_PATH, sa);

  isOk = load(&sm);

  notifyReload();

  return isOk;
}

bool Configurator::save(bool forService)
{
  bool isOk = false;

  HKEY rootKey = forService ? HKEY_LOCAL_MACHINE : HKEY_CURRENT_USER;

  SECURITY_ATTRIBUTES *sa = 0;
  if (forService && m_regSA != 0) {
    sa = m_regSA->getServiceSA();
  }
  RegistrySettingsManager sm(rootKey, RegistryPaths::SERVER_PATH, sa);

  isOk = save(&sm);

  return isOk;
}

bool Configurator::save(SettingsManager *sm)
{
  bool saveResult = true;
  if (!savePortMappingContainer(sm)) {
    saveResult = false;
  }
  if (!saveQueryConfig(sm)) {
    saveResult = false;
  }
  if (!saveInputHandlingConfig(sm)) {
    saveResult = false;
  }
  if (!saveIpAccessControlContainer(sm)) {
    saveResult = false;
  }
  if (!saveServerConfig(sm)) {
    saveResult = false;
  }
  if (!saveVideoRegionConfig(sm)) {
    saveResult = false;
  }
  return saveResult;
}

bool Configurator::load(SettingsManager *sm)
{
  bool loadResult = true;

  {
    AutoLock l(&m_serverConfig);

    if (!loadPortMappingContainer(sm, m_serverConfig.getPortMappingContainer())) {
      loadResult = false;
    }
  }

  if (!loadQueryConfig(sm, &m_serverConfig)) {
    loadResult = false;
  }
  if (!loadInputHandlingConfig(sm, &m_serverConfig)) {
    loadResult = false;
  }

  {
    AutoLock l(&m_serverConfig);

    if (!loadIpAccessControlContainer(sm, m_serverConfig.getAccessControl())) {
      loadResult = false;
    }
  }

  if (!loadServerConfig(sm, &m_serverConfig)) {
    loadResult = false;
  }
  if (!loadVideoRegionConfig(sm, &m_serverConfig)) {
    loadResult = false;
  }

  m_isFirstLoad = false;

  return loadResult;
}

bool Configurator::savePortMappingContainer(SettingsManager *sm)
{
  bool saveResult = true;

  //
  // Get port mappings from server config
  //

  AutoLock l(&m_serverConfig);

  PortMappingContainer *portMappings = m_serverConfig.getPortMappingContainer();

  size_t count = portMappings->count();
  StringStorage portMappingsString;
  StringStorage portMappingString;

  //
  // Create string to serialize
  //

  portMappingsString.setString(_T(""));
  for (size_t i = 0; i < count; i++) {
    const PortMapping *portMapping = portMappings->at(i);
    portMapping->toString(&portMappingString);
    portMappingsString.appendString(portMappingString.getString());
    if (i != count - 1) {
      portMappingsString.appendString(_T(","));
    }
  }

  //
  // Save port mappings
  //

  if (!sm->setString(_T("ExtraPorts"), portMappingsString.getString())) {
    saveResult = false;
  }
  return saveResult;
}

bool Configurator::loadPortMappingContainer(SettingsManager *sm,
                                            PortMappingContainer *portMapping)
{
  bool wasError = false;

  portMapping->removeAll();

  StringStorage extraPorts;

  if (!sm->getString(_T("ExtraPorts"), &extraPorts)) {
    return false;
  }

  size_t count = 0;

  extraPorts.split(_T(","), NULL, &count);
  if (count != 0) {
    std::vector<StringStorage> chunks(count);
    extraPorts.split(_T(","), &chunks.front(), &count);

    PortMapping mapping;

    for (size_t i = 0; i < count; i++) {
      if (PortMapping::parse(chunks[i].getString(), &mapping)) {
        portMapping->pushBack(mapping);
      } else {
        wasError = true;
      }
    }
  }

  return !wasError;
}

bool Configurator::saveQueryConfig(SettingsManager *sm)
{
  bool saveResult = true;
  if (!sm->setUINT(_T("QueryTimeout"), m_serverConfig.getQueryTimeout())) {
    saveResult = false;
  }
  if (!sm->setBoolean(_T("QueryAcceptOnTimeout"), m_serverConfig.isDefaultActionAccept())) {
    saveResult = false;
  }
  return saveResult;
}

bool Configurator::loadQueryConfig(SettingsManager *sm, ServerConfig *config)
{
  bool loadResult = true;
  unsigned int uintValue;
  bool boolValue;
  if (!sm->getUINT(_T("QueryTimeout"), &uintValue)) {
    loadResult = false;
  } else {
    m_isConfigLoadedPartly = true;
    m_serverConfig.setQueryTimeout(uintValue);
  }
  if (!sm->getBoolean(_T("QueryAcceptOnTimeout"), &boolValue)) {
    loadResult = false;
  } else {
    m_isConfigLoadedPartly = true;
    m_serverConfig.setDefaultActionToAccept(boolValue);
  }
  return loadResult;
}

bool Configurator::saveInputHandlingConfig(SettingsManager *sm)
{
  bool saveResult = true;
  if (!sm->setUINT(_T("LocalInputPriorityTimeout"), m_serverConfig.getLocalInputPriorityTimeout())) {
    saveResult = false;
  }
  if (!sm->setBoolean(_T("LocalInputPriority"), m_serverConfig.isLocalInputPriorityEnabled())) {
    saveResult = false;
  }
  if (!sm->setBoolean(_T("BlockRemoteInput"), m_serverConfig.isBlockingRemoteInput())) {
    saveResult = false;
  }
  if (!sm->setBoolean(_T("BlockLocalInput"), m_serverConfig.isBlockingLocalInput())) {
    saveResult = false;
  }
  return saveResult;
}

bool Configurator::loadInputHandlingConfig(SettingsManager *sm, ServerConfig *config)
{
  bool loadResult = true;

  //
  // Temporary variables
  //

  bool boolVal = false;
  UINT uintVal = 0;

  if (!sm->getUINT(_T("LocalInputPriorityTimeout"), &uintVal)) {
    loadResult = false;
  } else {
    m_isConfigLoadedPartly = true;
    config->setLocalInputPriorityTimeout(uintVal);
  }
  if (!sm->getBoolean(_T("LocalInputPriority"), &boolVal)) {
    loadResult = false;
  } else {
    m_isConfigLoadedPartly = true;
    config->setLocalInputPriority(boolVal);
  }
  if (!sm->getBoolean(_T("BlockRemoteInput"), &boolVal)) {
    loadResult = false;
  } else {
    m_isConfigLoadedPartly = true;
    config->blockRemoteInput(boolVal);
  }
  if (!sm->getBoolean(_T("BlockLocalInput"), &boolVal)) {
    loadResult = false;
  } else {
    m_isConfigLoadedPartly = true;
    config->blockLocalInput(boolVal);
  }

  return loadResult;
}

bool Configurator::saveVideoRegionConfig(SettingsManager *sm)
{
  bool saveResult = true;

  StringStorage buffer;
  StringVector *videoClasses = m_serverConfig.getVideoClassNames();
  size_t size = videoClasses->size();
 std::vector<Rect> *videoRects = m_serverConfig.getVideoRects();

  AutoLock al(&m_serverConfig);
  buffer.setString(_T(""));
  for (size_t i = 0; i < size; i++) {
    buffer.appendString(videoClasses->at(i).getString());
    if (i != size - 1) {
      buffer.appendString(_T("\n"));
    }
  }
  if (!sm->setString(_T("VideoClasses"), buffer.getString())) {
    saveResult = false;
  }
 
  size = videoRects->size();
  buffer.setString(_T(""));
  for (size_t i = 0; i < size; i++) {
    StringStorage s;
    RectSerializer::toString(&(videoRects->at(i)),&s);
    buffer.appendString(s.getString());
    if (i != size - 1) {
      buffer.appendString(_T("\n"));
    }
  }
  if (!sm->setString(_T("VideoRects"), buffer.getString())) {
    saveResult = false;
  }
  return saveResult;
}

bool Configurator::loadVideoRegionConfig(SettingsManager *sm, ServerConfig *config)
{
  bool loadResult = true;

  StringVector *videoClasses = m_serverConfig.getVideoClassNames();
  std::vector<Rect> *videoRects = m_serverConfig.getVideoRects();

  // Lock configuration
  AutoLock al(&m_serverConfig);

  //
  // Delete old video classes entries
  //

  videoClasses->clear();

  //
  // Try to load.
  //

  StringStorage storage;

  if (!sm->getString(_T("VideoClasses"), &storage)) {
    loadResult = false;
  }

  //
  // Split.
  //

  size_t count = 0;

  storage.split(_T("\n\t ,;"), NULL, &count);
  if (count != 0) {
    std::vector<StringStorage> chunks(count);
    storage.split(_T("\n\t ,;"), &chunks.front(), &count);

    for (size_t i = 0; i < count; i++) {
      if (!chunks[i].isEmpty()) {
        videoClasses->push_back(chunks[i]);
      }
    }
  }

  // Clear storage content
  storage.setString(_T(""));

  //
  // Delete old video rects entries
  //

  videoRects->clear();

  //
  // Try to load.
  //
  
  if (!sm->getString(_T("VideoRects"), &storage)) {
    loadResult = false;
  }

  //
  // Split.
  //

  count = 0;

  storage.split(_T("\n\t ,;"), NULL, &count);
  if (count != 0) {
    std::vector<StringStorage> chunks(count);
    storage.split(_T("\n\t ,;"), &chunks.front(), &count);

    for (size_t i = 0; i < count; i++) {
      if (!chunks[i].isEmpty()) {
        videoRects->push_back(RectSerializer::toRect(&chunks[i]));
      }
    }
  }
  return loadResult;
}

bool Configurator::saveIpAccessControlContainer(SettingsManager *storage)
{
  AutoLock l(&m_serverConfig);

  // Get rules container
  IpAccessControl *rules = m_serverConfig.getAccessControl();
  // Remember rules count
  size_t rulesCount = rules->size();
  // 1 rule can contain 34 character max
  size_t maxStringBufferLength = 34 * 2 * rulesCount;
  // Buffer that we need to write to storage
  StringStorage buffer(_T(""));
  // Variable to save temporary result from toString method
  StringStorage ruleString;

  // Generate rules string
  for (size_t i = 0; i < rulesCount; i++) {
    IpAccessRule *rule = rules->at(i);
    // Get rule as string
    rule->toString(&ruleString);
    // Add it to result buffer
    buffer.appendString(ruleString.getString());
    // Add delimiter if we need it
    if (i != rulesCount - 1)
      buffer.appendString(_T(","));
  }
  if (!storage->setString(_T("IpAccessControl"), buffer.getString())) {
    return false;
  }
  return true;
}

bool
Configurator::loadIpAccessControlContainer(SettingsManager *sm, IpAccessControl *rules)
{
  bool wasError = false;
  rules->clear();

  StringStorage storage;
  if (!sm->getString(_T("IpAccessControl"), &storage)) {
    return false;
  } else {
    size_t maxBufSize = storage.getLength() + 1;
    std::vector<TCHAR> ipacStringBuffer(maxBufSize + 1);
    _tcscpy_s(&ipacStringBuffer.front(), maxBufSize, storage.getString());
    TCHAR *pch = _tcstok(&ipacStringBuffer[0], _T(","));
    while (pch != NULL) {
      if (IpAccessRule::parse(pch, NULL)) {
        IpAccessRule *rule = new IpAccessRule();
        IpAccessRule::parse(pch, rule);
        rules->push_back(rule);
      } else {
        wasError = true;
      }
      pch = _tcstok(NULL, _T(","));
    } // while
  } // else
  return !wasError;
}

bool Configurator::saveServerConfig(SettingsManager *sm)
{
  bool saveResult = true;
  if (!sm->setUINT(_T("RfbPort"), m_serverConfig.getRfbPort())) {
    saveResult = false;
  }
  if (!sm->setUINT(_T("HttpPort"), m_serverConfig.getHttpPort())) {
    saveResult = false;
  }
  if (!sm->setUINT(_T("DisconnectAction"), (UINT)m_serverConfig.getDisconnectAction())) {
    saveResult = false;
  }
  if (!sm->setBoolean(_T("AcceptRfbConnections"), m_serverConfig.isAcceptingRfbConnections())) {
    saveResult = false;
  }
  if (!sm->setBoolean(_T("UseVncAuthentication"), m_serverConfig.isUsingAuthentication())) {
    saveResult = false;
  }
  if (!sm->setBoolean(_T("UseControlAuthentication"), m_serverConfig.isControlAuthEnabled())) {
    saveResult = false;
  }
  if (!sm->setBoolean(_T("RepeatControlAuthentication"), m_serverConfig.getControlAuthAlwaysChecking())) {
    saveResult = false;
  }
  if (!sm->setBoolean(_T("LoopbackOnly"), m_serverConfig.isOnlyLoopbackConnectionsAllowed())) {
    saveResult = false;
  }
  if (!sm->setBoolean(_T("AcceptHttpConnections"), m_serverConfig.isAcceptingHttpConnections())) {
    saveResult = false;
  }
  if (!sm->setUINT(_T("LogLevel"), (UINT)m_serverConfig.getLogLevel())) {
    saveResult = false;
  }
  if (!sm->setBoolean(_T("EnableFileTransfers"), m_serverConfig.isFileTransfersEnabled())) {
    saveResult = false;
  }
  if (!sm->setBoolean(_T("RemoveWallpaper"), m_serverConfig.isRemovingDesktopWallpaperEnabled())) {
    saveResult = false;
  }
  if (!sm->setBoolean(_T("UseD3D"), m_serverConfig.getD3DIsAllowed())) {
    saveResult = false;
  }
  if (!sm->setBoolean(_T("UseMirrorDriver"), m_serverConfig.getMirrorIsAllowed())) {
    saveResult = false;
  }
  if (!sm->setBoolean(_T("EnableUrlParams"), m_serverConfig.isAppletParamInUrlEnabled())) {
    saveResult = false;
  }
  if (m_serverConfig.hasPrimaryPassword()) {
    unsigned char password[ServerConfig::VNC_PASSWORD_SIZE];

    m_serverConfig.getPrimaryPassword(&password[0]);

    if (!sm->setBinaryData(_T("Password"), &password[0], ServerConfig::VNC_PASSWORD_SIZE)) {
      saveResult = false;
    }
  } else {
    sm->deleteKey(_T("Password"));
  }
  if (m_serverConfig.hasReadOnlyPassword()) {
    unsigned char password[ServerConfig::VNC_PASSWORD_SIZE];

    m_serverConfig.getReadOnlyPassword(&password[0]);

    if (!sm->setBinaryData(_T("PasswordViewOnly"), &password[0], ServerConfig::VNC_PASSWORD_SIZE)) {
      saveResult = false;
    }
  } else {
    sm->deleteKey(_T("PasswordViewOnly"));
  }
  if (m_serverConfig.hasControlPassword()) {
    unsigned char password[ServerConfig::VNC_PASSWORD_SIZE];

    m_serverConfig.getControlPassword(&password[0]);

    if (!sm->setBinaryData(_T("ControlPassword"), &password[0], ServerConfig::VNC_PASSWORD_SIZE)) {
      saveResult = false;
    }
  } else {
    sm->deleteKey(_T("ControlPassword"));
  }
  if (!sm->setBoolean(_T("AlwaysShared"), m_serverConfig.isAlwaysShared())) {
    saveResult = false;
  }
  if (!sm->setBoolean(_T("NeverShared"), m_serverConfig.isNeverShared())) {
    saveResult = false;
  }
  if (!sm->setBoolean(_T("DisconnectClients"), m_serverConfig.isDisconnectingExistingClients())) {
    saveResult = false;
  }
  if (!sm->setUINT(_T("PollingInterval"), m_serverConfig.getPollingInterval())) {
    saveResult = false;
  }
  if (!sm->setBoolean(_T("AllowLoopback"), m_serverConfig.isLoopbackConnectionsAllowed())) {
    saveResult = false;
  }
  if (!sm->setUINT(_T("VideoRecognitionInterval"), m_serverConfig.getVideoRecognitionInterval())) {
    saveResult = false;
  }
  if (!sm->setBoolean(_T("GrabTransparentWindows"), m_serverConfig.getGrabTransparentWindowsFlag())) {
    saveResult = false;
  }
  if (!sm->setBoolean(_T("SaveLogToAllUsersPath"), m_serverConfig.isSaveLogToAllUsersPathFlagEnabled())) {
    saveResult = false;
  }
  if (!sm->setBoolean(_T("RunControlInterface"), m_serverConfig.getShowTrayIconFlag())) {
    saveResult = false;
  }
  if (!sm->setUINT(_T("IdleTimeout"), (UINT)m_serverConfig.getIdleTimeout())) {
    saveResult = false;
  }
  return saveResult;
}

bool Configurator::loadServerConfig(SettingsManager *sm, ServerConfig *config)
{
  bool loadResult = true;

  //
  // Temporary variables
  //

  bool boolVal;
  UINT uintVal;

  if (!sm->getUINT(_T("RfbPort"), &uintVal)) {
    loadResult = false;
  } else {
    m_serverConfig.setRfbPort(uintVal);
  }
  if (!sm->getUINT(_T("HttpPort"), &uintVal)) {
    loadResult = false;
  } else {
    m_isConfigLoadedPartly = true;
    m_serverConfig.setHttpPort(uintVal);
  }
  if (!sm->getUINT(_T("DisconnectAction"), &uintVal)) {
    loadResult = false;
  } else {
    m_isConfigLoadedPartly = true;
    m_serverConfig.setDisconnectAction((ServerConfig::DisconnectAction)uintVal);
  }
  if (!sm->getBoolean(_T("AcceptRfbConnections"), &boolVal)) {
    loadResult = false;
  } else {
    m_isConfigLoadedPartly = true;
    m_serverConfig.acceptRfbConnections(boolVal);
  }
  if (!sm->getBoolean(_T("UseVncAuthentication"), &boolVal)) {
    loadResult = false;
  } else {
    m_isConfigLoadedPartly = true;
    m_serverConfig.useAuthentication(boolVal);
  }
  if (!sm->getBoolean(_T("UseControlAuthentication"), &boolVal)) {
    loadResult = false;
  } else {
    m_isConfigLoadedPartly = true;
    m_serverConfig.useControlAuth(boolVal);
  }
  if (!sm->getBoolean(_T("RepeatControlAuthentication"), &boolVal)) {
    loadResult = false;
  } else {
    m_isConfigLoadedPartly = true;
    m_serverConfig.setControlAuthAlwaysChecking(boolVal);
  }
  if (!sm->getBoolean(_T("LoopbackOnly"), &boolVal)) {
    loadResult = false;
  } else {
    m_isConfigLoadedPartly = true;
    m_serverConfig.acceptOnlyLoopbackConnections(boolVal);
  }
  if (!sm->getBoolean(_T("AcceptHttpConnections"), &boolVal)) {
    loadResult = false;
  } else {
    m_isConfigLoadedPartly = true;
    m_serverConfig.acceptHttpConnections(boolVal);
  }
  if (!sm->getUINT(_T("LogLevel"), &uintVal)) {
    loadResult = false;
  } else {
    m_isConfigLoadedPartly = true;
    m_serverConfig.setLogLevel(uintVal);
  }
  if (!sm->getBoolean(_T("EnableFileTransfers"), &boolVal)) {
    loadResult = false;
  } else {
    m_isConfigLoadedPartly = true;
    m_serverConfig.enableFileTransfers(boolVal);
  }
  if (!sm->getBoolean(_T("RemoveWallpaper"), &boolVal)) {
    loadResult = false;
  } else {
    m_isConfigLoadedPartly = true;
    m_serverConfig.enableRemovingDesktopWallpaper(boolVal);
  }
  if (!sm->getBoolean(_T("UseD3D"), &boolVal)) {
    loadResult = false;
  }
  else {
    m_isConfigLoadedPartly = true;
    m_serverConfig.setD3DAllowing(boolVal);
  }
  if (!sm->getBoolean(_T("UseMirrorDriver"), &boolVal)) {
    loadResult = false;
  } else {
    m_isConfigLoadedPartly = true;
    m_serverConfig.setMirrorAllowing(boolVal);
  }
  if (!sm->getBoolean(_T("EnableUrlParams"), &boolVal)) {
    loadResult = false;
  } else {
    m_isConfigLoadedPartly = true;
    m_serverConfig.enableAppletParamInUrl(boolVal);
  }

  size_t passSize = 8;
  unsigned char buffer[ServerConfig::VNC_PASSWORD_SIZE] = {0};

  if (!sm->getBinaryData(_T("Password"), (void *)&buffer, &passSize)) {
    loadResult = false;
    m_serverConfig.deletePrimaryPassword();
  } else {
    m_isConfigLoadedPartly = true;
    m_serverConfig.setPrimaryPassword(&buffer[0]);
  }
  passSize = 8;
  if (!sm->getBinaryData(_T("PasswordViewOnly"), (void *)&buffer, &passSize)) {
    loadResult = false;
    m_serverConfig.deleteReadOnlyPassword();
  } else {
    m_isConfigLoadedPartly = true;
    m_serverConfig.setReadOnlyPassword(&buffer[0]);
  }
  passSize = 8;
  if (!sm->getBinaryData(_T("ControlPassword"), (void *)&buffer, &passSize)) {
    loadResult = false;
    m_serverConfig.deleteControlPassword();
  } else {
    m_isConfigLoadedPartly = true;
    m_serverConfig.setControlPassword(&buffer[0]);
  }

  if (!sm->getBoolean(_T("AlwaysShared"), &boolVal)) {
    loadResult = false;
  } else {
    m_isConfigLoadedPartly = true;
    m_serverConfig.setAlwaysShared(boolVal);
  }
  if (!sm->getBoolean(_T("NeverShared"), &boolVal)) {
    loadResult = false;
  } else {
    m_isConfigLoadedPartly = true;
    m_serverConfig.setNeverShared(boolVal);
  }
  if (!sm->getBoolean(_T("DisconnectClients"), &boolVal)) {
    loadResult = false;
  } else {
    m_isConfigLoadedPartly = true;
    m_serverConfig.disconnectExistingClients(boolVal);
  }
  if (!sm->getUINT(_T("PollingInterval"), &uintVal)) {
    loadResult = false;
  } else {
    m_isConfigLoadedPartly = true;
    m_serverConfig.setPollingInterval(uintVal);
  }
  if (!sm->getBoolean(_T("AllowLoopback"), &boolVal)) {
    loadResult = false;
  } else {
    m_isConfigLoadedPartly = true;
    m_serverConfig.allowLoopbackConnections(boolVal);
  }
  if (!sm->getUINT(_T("VideoRecognitionInterval"), &uintVal)) {
    loadResult = false;
  } else {
    m_isConfigLoadedPartly = true;
    m_serverConfig.setVideoRecognitionInterval(uintVal);
  }
  if (!sm->getUINT(_T("IdleTimeout"), &uintVal)) {
    loadResult = false;
  } else {
    m_isConfigLoadedPartly = true;
    m_serverConfig.setIdleTimeout((int)uintVal);
  }
  if (!sm->getBoolean(_T("GrabTransparentWindows"), &boolVal)) {
    loadResult = false;
  } else {
    m_isConfigLoadedPartly = true;
    m_serverConfig.setGrabTransparentWindowsFlag(boolVal);
  }
  if (!sm->getBoolean(_T("SaveLogToAllUsersPath"), &boolVal)) {
    loadResult = false;
  } else {
    m_isConfigLoadedPartly = true;
    m_serverConfig.saveLogToAllUsersPath(boolVal);
  }
  if (!sm->getBoolean(_T("RunControlInterface"), &boolVal)) {
    loadResult = false;
  } else {
    m_isConfigLoadedPartly = true;
    m_serverConfig.setShowTrayIconFlag(boolVal);
  }
  updateLogDirPath();
  return loadResult;
}

void Configurator::updateLogDirPath()
{
  StringStorage pathToLogDirectory;
  TvnLogFilename::queryLogFileDirectory(m_isConfiguringService,
    m_serverConfig.isSaveLogToAllUsersPathFlagEnabled(),
    &pathToLogDirectory);
  m_serverConfig.setLogFileDir(pathToLogDirectory.getString());
}
