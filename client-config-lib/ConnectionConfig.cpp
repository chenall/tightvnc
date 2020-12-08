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

#include "ConnectionConfig.h"

#include "thread/AutoLock.h"

#include <crtdbg.h>

#define TEST_FAIL(C,R) if (!C) { R = false; }

ConnectionConfig::ConnectionConfig()
: m_swapMouse(false), m_viewOnly(false),
  m_useFullscreen(false), m_use8BitColor(false), m_preferredEncoding(EncodingDefs::TIGHT),
  m_requestSharedSession(true), m_deiconifyOnRemoteBell(false),
  m_isClipboardEnabled(true),
  m_customCompressionLevel(-1), m_jpegCompressionLevel(6),
  m_fitWindow(false), m_requestShapeUpdates(true),
  m_ignoreShapeUpdates(false), m_scaleNumerator(1), m_scaleDenominator(1),
  m_localCursor(DOT_CURSOR), m_allowedCopyRect(true)
{
}

ConnectionConfig::~ConnectionConfig()
{
}

ConnectionConfig::ConnectionConfig(const ConnectionConfig &connectionConfig)
{
  *this = connectionConfig;
}

ConnectionConfig& ConnectionConfig::operator=(const ConnectionConfig& other)
{
  bool allowedCopyRect;
  unsigned char preferredEncoding;
  bool use8BitColor;
  int customCompressionLevel;
  int jpegCompressionLevel;
  bool viewOnly;
  bool isClipboardEnabled;
  bool useFullscreen;
  bool deiconifyOnRemoteBell;
  int scaleNumerator;
  int scaleDenominator;
  bool swapMouse;
  bool requestSharedSession;
  bool fitWindow;
  bool requestShapeUpdates;
  bool ignoreShapeUpdates;
  int localCursor;

  {
    AutoLock lockOther(&other.m_cs);
    allowedCopyRect = other.m_allowedCopyRect;
    preferredEncoding = other.m_preferredEncoding;
    use8BitColor = other.m_use8BitColor;
    customCompressionLevel = other.m_customCompressionLevel;
    jpegCompressionLevel = other.m_jpegCompressionLevel;
    viewOnly = other.m_viewOnly;
    isClipboardEnabled = other.m_isClipboardEnabled;
    useFullscreen = other.m_useFullscreen;
    deiconifyOnRemoteBell = other.m_deiconifyOnRemoteBell;
    scaleNumerator = other.m_scaleNumerator;
    scaleDenominator = other.m_scaleDenominator;
    swapMouse = other.m_swapMouse;
    requestSharedSession = other.m_requestSharedSession;
    fitWindow = other.m_fitWindow;
    requestShapeUpdates = other.m_requestShapeUpdates;
    ignoreShapeUpdates = other.m_ignoreShapeUpdates;
    localCursor = other.m_localCursor;
  }

  {
    AutoLock lockThis(&m_cs);
    m_allowedCopyRect = allowedCopyRect;
    m_preferredEncoding = preferredEncoding;
    m_use8BitColor = use8BitColor;
    m_customCompressionLevel = customCompressionLevel;
    m_jpegCompressionLevel = jpegCompressionLevel;
    m_viewOnly = viewOnly;
    m_isClipboardEnabled = isClipboardEnabled;
    m_useFullscreen = useFullscreen;
    m_deiconifyOnRemoteBell = deiconifyOnRemoteBell;
    m_scaleNumerator = scaleNumerator;
    m_scaleDenominator = scaleDenominator;
    m_swapMouse = swapMouse;
    m_requestSharedSession = requestSharedSession;
    m_fitWindow = fitWindow;
    m_requestShapeUpdates = requestShapeUpdates;
    m_ignoreShapeUpdates = ignoreShapeUpdates;
    m_localCursor = localCursor;
  }
  return *this;
}

void ConnectionConfig::allowCopyRect(bool allow)
{
  AutoLock l(&m_cs);
  m_allowedCopyRect = allow;
}

bool ConnectionConfig::isCopyRectAllowed()
{
  AutoLock l(&m_cs);
  return m_allowedCopyRect;
}

void ConnectionConfig::setPreferredEncoding(int encoding)
{
  AutoLock l(&m_cs);
  m_preferredEncoding = encoding;
}

int ConnectionConfig::getPreferredEncoding()
{
  AutoLock l(&m_cs);
  return m_preferredEncoding;
}

void ConnectionConfig::use8BitColor(bool use)
{
  AutoLock l(&m_cs);
  m_use8BitColor = use;
}

bool ConnectionConfig::isUsing8BitColor()
{
  AutoLock l(&m_cs);
  return m_use8BitColor;
}

void ConnectionConfig::setCustomCompressionLevel(int level)
{
  // level in interval [0..9]
  // if compression is disable, then level is -1.
  if (level < -1)
    level = 0;
  if (level > 9)
    level = 9;

  {
    AutoLock l(&m_cs);
    m_customCompressionLevel = level;
  }
}

void ConnectionConfig::setJpegCompressionLevel(int level)
{
  // level in interval [0..9]
  // if jpeg compression is disable, then level is -1.
  if (level < -1)
      level = 0;
  if (level > 9)
    level = 9;

  {
    AutoLock l(&m_cs);
    m_jpegCompressionLevel = level;
  }
}

int ConnectionConfig::getCustomCompressionLevel()
{
  AutoLock l(&m_cs);
  return m_customCompressionLevel;
}

int ConnectionConfig::getJpegCompressionLevel()
{
  AutoLock l(&m_cs);
  return m_jpegCompressionLevel;
}

bool ConnectionConfig::isCustomCompressionEnabled()
{
  return getCustomCompressionLevel() != -1;
}

bool ConnectionConfig::isJpegCompressionEnabled()
{
  return getJpegCompressionLevel() != -1;
}

void ConnectionConfig::disableCustomCompression()
{
  setCustomCompressionLevel(-1);
}

void ConnectionConfig::disableJpegCompression()
{
  setJpegCompressionLevel(-1);
}

void ConnectionConfig::setViewOnly(bool viewOnly)
{
  AutoLock l(&m_cs);
  m_viewOnly = viewOnly;
}

bool ConnectionConfig::isViewOnly()
{
  AutoLock l(&m_cs);
  return m_viewOnly;
}

void ConnectionConfig::enableClipboard(bool enabled)
{
  AutoLock l(&m_cs);
  m_isClipboardEnabled = enabled;
}

bool ConnectionConfig::isClipboardEnabled()
{
  AutoLock l(&m_cs);
  return m_isClipboardEnabled;
}

void ConnectionConfig::enableFullscreen(bool useFullscreen)
{
  AutoLock l(&m_cs);
  m_useFullscreen = useFullscreen;
}

bool ConnectionConfig::isFullscreenEnabled()
{
  AutoLock l(&m_cs);
  return m_useFullscreen;
}

void ConnectionConfig::deiconifyOnRemoteBell(bool deiconifyFlag)
{
  AutoLock l(&m_cs);
  m_deiconifyOnRemoteBell = deiconifyFlag;
}

bool ConnectionConfig::isDeiconifyOnRemoteBellEnabled()
{
  AutoLock l(&m_cs);
  return m_deiconifyOnRemoteBell;
}

int ConnectionConfig::getScaleNumerator()
{
  AutoLock l(&m_cs);
  return m_scaleNumerator;
}

int ConnectionConfig::getScaleDenominator()
{
  AutoLock l(&m_cs);
  return m_scaleDenominator;
}

void ConnectionConfig::setScale(int n, int d)
{
  if ((n < 1) || (d < 1)) {
    n = d = 1;
  }

  if ((n * 100.0) / d > 400) {
    n = 400;
    d = 100;
  }

  int g = gcd(n, d);

  n /= g;
  d /= g;

  {
    AutoLock l(&m_cs);
    m_scaleNumerator = n;
    m_scaleDenominator = d;
  }
}

void ConnectionConfig::swapMouse(bool enabled)
{
  AutoLock l(&m_cs);
  m_swapMouse = enabled;
}

bool ConnectionConfig::isMouseSwapEnabled()
{
  AutoLock l(&m_cs);
  return m_swapMouse;
}

void ConnectionConfig::setSharedFlag(bool shared)
{
  AutoLock l(&m_cs);
  m_requestSharedSession = shared;
}

bool ConnectionConfig::getSharedFlag()
{
  AutoLock l(&m_cs);
  return m_requestSharedSession;
}

void ConnectionConfig::fitWindow(bool enabled)
{
  AutoLock l(&m_cs);
  m_fitWindow = enabled;
}

bool ConnectionConfig::isFitWindowEnabled()
{
  AutoLock l(&m_cs);
  return m_fitWindow;
}

void ConnectionConfig::requestShapeUpdates(bool requestFlag)
{
  AutoLock l(&m_cs);
  m_requestShapeUpdates = requestFlag;
}

void ConnectionConfig::ignoreShapeUpdates(bool ignoreFlag)
{
  AutoLock l(&m_cs);
  m_ignoreShapeUpdates = ignoreFlag;
}

bool ConnectionConfig::isRequestingShapeUpdates()
{
  AutoLock l(&m_cs);
  return m_requestShapeUpdates;
}

bool ConnectionConfig::isIgnoringShapeUpdates()
{
  AutoLock l(&m_cs);
  return m_ignoreShapeUpdates;
}

void ConnectionConfig::setLocalCursorShape(int cursorShape)
{
  switch (cursorShape) {
  case NO_CURSOR:
  case DOT_CURSOR:
  case NORMAL_CURSOR:
  case SMALL_CURSOR:
    break;
  default:
    cursorShape = DOT_CURSOR;
    _ASSERT(FALSE);
  } // switch

  {
    AutoLock l(&m_cs);
    m_localCursor = cursorShape;
  }
}

int ConnectionConfig::getLocalCursorShape()
{
  AutoLock l(&m_cs);
  return m_localCursor;
}

bool ConnectionConfig::saveToStorage(SettingsManager *sm) const
{
  AutoLock l(&m_cs);

  bool saveAllOk = true;

  // option "use_encoding_1" have need for compatible with old-vnc files
  TEST_FAIL(sm->setBoolean(_T("use_encoding_1"),   m_allowedCopyRect), saveAllOk);
  TEST_FAIL(sm->setBoolean(_T("copyrect"),         m_allowedCopyRect), saveAllOk);
  TEST_FAIL(sm->setBoolean(_T("viewonly"),         m_viewOnly), saveAllOk);
  TEST_FAIL(sm->setBoolean(_T("fullscreen"),       m_useFullscreen), saveAllOk);
  TEST_FAIL(sm->setBoolean(_T("8bit"),             m_use8BitColor), saveAllOk);
  TEST_FAIL(sm->setBoolean(_T("shared"),           m_requestSharedSession), saveAllOk);
  TEST_FAIL(sm->setBoolean(_T("belldeiconify"),    m_deiconifyOnRemoteBell), saveAllOk);
  TEST_FAIL(sm->setBoolean(_T("disableclipboard"), !m_isClipboardEnabled), saveAllOk);
  TEST_FAIL(sm->setBoolean(_T("swapmouse"),        m_swapMouse), saveAllOk);
  TEST_FAIL(sm->setBoolean(_T("fitwindow"),        m_fitWindow), saveAllOk);
  TEST_FAIL(sm->setBoolean(_T("cursorshape"),      m_requestShapeUpdates), saveAllOk);
  TEST_FAIL(sm->setBoolean(_T("noremotecursor"),   m_ignoreShapeUpdates), saveAllOk);

  TEST_FAIL(sm->setByte(_T("preferred_encoding"),  m_preferredEncoding), saveAllOk);
  TEST_FAIL(sm->setInt(_T("compresslevel"),        m_customCompressionLevel), saveAllOk);
  TEST_FAIL(sm->setInt(_T("quality"),              m_jpegCompressionLevel), saveAllOk);
  TEST_FAIL(sm->setInt(_T("localcursor"),          m_localCursor), saveAllOk);
  TEST_FAIL(sm->setInt(_T("scale_den"),            m_scaleDenominator), saveAllOk);
  TEST_FAIL(sm->setInt(_T("scale_num"),            m_scaleNumerator), saveAllOk);

  TEST_FAIL(sm->setInt(_T("local_cursor_shape"),   m_localCursor), saveAllOk);

  return saveAllOk;
}

bool ConnectionConfig::loadFromStorage(SettingsManager *sm)
{
  AutoLock l(&m_cs);

  bool loadAllOk = true;


  // option "use_encoding_1" have need for compatible with old-vnc files
  TEST_FAIL(sm->getBoolean(_T("use_encoding_1"),   &m_allowedCopyRect), loadAllOk);
  TEST_FAIL(sm->getBoolean(_T("copyrect"),         &m_allowedCopyRect), loadAllOk);
  TEST_FAIL(sm->getBoolean(_T("viewonly"),         &m_viewOnly), loadAllOk);
  TEST_FAIL(sm->getBoolean(_T("fullscreen"),       &m_useFullscreen), loadAllOk);
  TEST_FAIL(sm->getBoolean(_T("8bit"),             &m_use8BitColor), loadAllOk);
  TEST_FAIL(sm->getBoolean(_T("shared"),           &m_requestSharedSession), loadAllOk);
  TEST_FAIL(sm->getBoolean(_T("belldeiconify"),    &m_deiconifyOnRemoteBell), loadAllOk);

  if (sm->getBoolean(_T("disableclipboard"), &m_isClipboardEnabled)) {
    m_isClipboardEnabled = !m_isClipboardEnabled;
  } else {
    loadAllOk = false;
  }

  TEST_FAIL(sm->getBoolean(_T("swapmouse"),        &m_swapMouse), loadAllOk);
  TEST_FAIL(sm->getBoolean(_T("fitwindow"),        &m_fitWindow), loadAllOk);
  TEST_FAIL(sm->getBoolean(_T("cursorshape"),      &m_requestShapeUpdates), loadAllOk);
  TEST_FAIL(sm->getBoolean(_T("noremotecursor"),   &m_ignoreShapeUpdates), loadAllOk);

  TEST_FAIL(sm->getByte(_T("preferred_encoding"),  (char *)&m_preferredEncoding), loadAllOk);

  TEST_FAIL(sm->getInt(_T("compresslevel"),        &m_customCompressionLevel), loadAllOk);
  TEST_FAIL(sm->getInt(_T("quality"),              &m_jpegCompressionLevel), loadAllOk);
  TEST_FAIL(sm->getInt(_T("localcursor"),          &m_localCursor), loadAllOk);
  TEST_FAIL(sm->getInt(_T("scale_den"),            &m_scaleDenominator), loadAllOk);
  TEST_FAIL(sm->getInt(_T("scale_num"),            &m_scaleNumerator), loadAllOk);

  TEST_FAIL(sm->getInt(_T("local_cursor_shape"),   &m_localCursor), loadAllOk);

  return loadAllOk;
}

// Greatest common denominator, by Euclid
int ConnectionConfig::gcd(int a, int b) const
{
  if (a < b) {
    return gcd(b, a);
  }
  if (b == 0) {
    return a;
  }
  return gcd(b, a % b);
}
