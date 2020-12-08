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

#include "EncodeOptions.h"
#include "rfb/EncodingDefs.h"

EncodeOptions::EncodeOptions()
{
  reset();
}

EncodeOptions::~EncodeOptions()
{
}

void EncodeOptions::reset()
{
  m_preferredEncoding = EncodingDefs::RAW;

  m_compressionLevel = EO_DEFAULT;
  m_jpegQualityLevel = EO_DEFAULT;

  m_enableRRE = false;
  m_enableHextile = false;
  m_enableZrle = false;
  m_enableTight = false;

  m_enableCopyRect = false;
  m_enableRichCursor = false;
  m_enablePointerPos = false;
  m_enableDesktopSize = false;
}

void EncodeOptions::setEncodings(std::vector<int> *list)
{
  reset();
  bool encoderWasSet = false;

  size_t numCodes = list->size();
  std::vector<int>::const_iterator i;
  for (i = list->begin(); i != list->end(); i++) {
    int code = *i;
    if (!encoderWasSet && normalEncoding(code)) {
      m_preferredEncoding = code;
      encoderWasSet = true;
    }
    if (code == EncodingDefs::TIGHT) {
      m_enableTight = true;
    } else if (code == EncodingDefs::ZRLE) {
      m_enableZrle = true;
    } else if (code == EncodingDefs::HEXTILE) {
      m_enableHextile = true;
    } else if (code == EncodingDefs::RRE) {
      m_enableRRE = true;
    } else if (code == EncodingDefs::COPYRECT) {
      m_enableCopyRect = true;
    } else if (code == PseudoEncDefs::RICH_CURSOR) {
      m_enableRichCursor = true;
    } else if (code == PseudoEncDefs::POINTER_POS) {
      m_enablePointerPos = true;
    } else if (code == PseudoEncDefs::DESKTOP_SIZE) {
      m_enableDesktopSize = true;
    } else if (code >= PseudoEncDefs::COMPR_LEVEL_0 &&
               code <= PseudoEncDefs::COMPR_LEVEL_9) {
      int level = code - PseudoEncDefs::COMPR_LEVEL_0;
      m_compressionLevel = level;
    } else if (code >= PseudoEncDefs::QUALITY_LEVEL_0 &&
               code <= PseudoEncDefs::QUALITY_LEVEL_9) {
      int level = code - PseudoEncDefs::QUALITY_LEVEL_0;
      m_jpegQualityLevel = level;
    }
  }
}

int EncodeOptions::getPreferredEncoding() const
{
  return m_preferredEncoding;
}

bool EncodeOptions::encodingEnabled(int code) const
{
  switch (code) {
  case EncodingDefs::RAW:
    return true;
  case EncodingDefs::RRE:
    return m_enableRRE;
  case EncodingDefs::HEXTILE:
    return m_enableHextile;
  case EncodingDefs::ZRLE:
    return m_enableZrle;
  case EncodingDefs::TIGHT:
    return m_enableTight;
  }
  return false;
}

int EncodeOptions::getCompressionLevel(int defaultLevel) const
{
  int wasSet = (m_compressionLevel != EO_DEFAULT);
  return wasSet ? m_compressionLevel : defaultLevel;
}

int EncodeOptions::getJpegQualityLevel(int defaultLevel) const
{
  return jpegEnabled() ? m_jpegQualityLevel : defaultLevel;
}

bool EncodeOptions::jpegEnabled() const
{
  return (m_jpegQualityLevel != EO_DEFAULT);
}

void EncodeOptions::disableJpeg()
{
  m_jpegQualityLevel = EO_DEFAULT;
}

bool EncodeOptions::copyRectEnabled() const
{
  return m_enableCopyRect;
}

bool EncodeOptions::richCursorEnabled() const
{
  return m_enableRichCursor;
}

bool EncodeOptions::pointerPosEnabled() const
{
  return m_enablePointerPos;
}

bool EncodeOptions::desktopSizeEnabled() const
{
  return m_enableDesktopSize;
}

bool EncodeOptions::normalEncoding(int code)
{
  return (code == EncodingDefs::RAW ||
          code == EncodingDefs::RRE ||
          code == EncodingDefs::HEXTILE ||
          code == EncodingDefs::ZRLE ||
          code == EncodingDefs::TIGHT);
}
