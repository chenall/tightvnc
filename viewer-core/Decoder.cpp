// Copyright (C) 2011,2012 GlavSoft LLC.
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

#include "Decoder.h"

Decoder::Decoder(LogWriter *logWriter)
: m_logWriter(logWriter)
{
}

Decoder::~Decoder()
{
}

int Decoder::getCode() const
{
  return m_encoding;
}

bool Decoder::isPseudo() const
{
  return isPseudo(getCode());
}

bool Decoder::isPseudo(int encoding)
{
  switch (encoding) {
  case EncodingDefs::RAW:
  case EncodingDefs::COPYRECT:
  case EncodingDefs::RRE:
  case EncodingDefs::HEXTILE:
  case EncodingDefs::TIGHT:
  case EncodingDefs::ZRLE:
    return false;

  case PseudoEncDefs::COMPR_LEVEL_0:
  case PseudoEncDefs::COMPR_LEVEL_1:
  case PseudoEncDefs::COMPR_LEVEL_2:
  case PseudoEncDefs::COMPR_LEVEL_3:
  case PseudoEncDefs::COMPR_LEVEL_4:
  case PseudoEncDefs::COMPR_LEVEL_5:
  case PseudoEncDefs::COMPR_LEVEL_6:
  case PseudoEncDefs::COMPR_LEVEL_7:
  case PseudoEncDefs::COMPR_LEVEL_8:
  case PseudoEncDefs::COMPR_LEVEL_9:

  case PseudoEncDefs::QUALITY_LEVEL_0:
  case PseudoEncDefs::QUALITY_LEVEL_1:
  case PseudoEncDefs::QUALITY_LEVEL_2:
  case PseudoEncDefs::QUALITY_LEVEL_3:
  case PseudoEncDefs::QUALITY_LEVEL_4:
  case PseudoEncDefs::QUALITY_LEVEL_5:
  case PseudoEncDefs::QUALITY_LEVEL_6:
  case PseudoEncDefs::QUALITY_LEVEL_7:
  case PseudoEncDefs::QUALITY_LEVEL_8:
  case PseudoEncDefs::QUALITY_LEVEL_9:

  case PseudoEncDefs::DESKTOP_SIZE:
  case PseudoEncDefs::X_CURSOR:
  case PseudoEncDefs::RICH_CURSOR:
  case PseudoEncDefs::POINTER_POS:
  case PseudoEncDefs::LAST_RECT:
    return true;
  default:
    // if the encoding type is unknown, then generate assertions.
    _ASSERT(TRUE);
    return true;
  }
}
