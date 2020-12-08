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

#include "JpegQualityLevel.h"

JpegQualityLevel::JpegQualityLevel(LogWriter *logWriter, int quality)
: PseudoDecoder(logWriter)
{
  m_encoding = qualityToEncoding(quality);
}

JpegQualityLevel::~JpegQualityLevel()
{
}

int JpegQualityLevel::qualityToEncoding(int qualityLevel)
{
  switch (qualityLevel) {
  case 0: return PseudoEncDefs::QUALITY_LEVEL_0;
  case 1: return PseudoEncDefs::QUALITY_LEVEL_1;
  case 2: return PseudoEncDefs::QUALITY_LEVEL_2;
  case 3: return PseudoEncDefs::QUALITY_LEVEL_3;
  case 4: return PseudoEncDefs::QUALITY_LEVEL_4;
  case 5: return PseudoEncDefs::QUALITY_LEVEL_5;
  case 6: return PseudoEncDefs::QUALITY_LEVEL_6;
  case 7: return PseudoEncDefs::QUALITY_LEVEL_7;
  case 8: return PseudoEncDefs::QUALITY_LEVEL_8;
  case 9: return PseudoEncDefs::QUALITY_LEVEL_9;
  default:
    StringStorage error;
    error.format(_T("Quality of jpeg \"%d\" is not valid"), qualityLevel);
    throw Exception(error.getString());
  }
}
