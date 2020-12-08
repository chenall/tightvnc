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

#ifndef __RFB_ENCODING_DEFS_H_INCLUDED__
#define __RFB_ENCODING_DEFS_H_INCLUDED__

//
// Codes for encoding types
//

class EncodingDefs
{
public:
  static const int RAW = 0;
  static const int COPYRECT = 1;
  static const int RRE = 2;
  static const int HEXTILE = 5;
  static const int TIGHT = 7;
  static const int ZRLE = 16;

  static const char *const SIG_RAW;
  static const char *const SIG_COPYRECT;
  static const char *const SIG_RRE;
  static const char *const SIG_HEXTILE;
  static const char *const SIG_TIGHT;
  static const char *const SIG_ZRLE;
};

//
// Codes for pseudo-encoding types
//

class PseudoEncDefs
{
public:
  static const int COMPR_LEVEL_0 = -256;
  static const int COMPR_LEVEL_1 = -255;
  static const int COMPR_LEVEL_2 = -254;
  static const int COMPR_LEVEL_3 = -253;
  static const int COMPR_LEVEL_4 = -252;
  static const int COMPR_LEVEL_5 = -251;
  static const int COMPR_LEVEL_6 = -250;
  static const int COMPR_LEVEL_7 = -249;
  static const int COMPR_LEVEL_8 = -248;
  static const int COMPR_LEVEL_9 = -247;

  static const int X_CURSOR = -240;
  static const int RICH_CURSOR = -239;
  static const int POINTER_POS = -232;

  static const int LAST_RECT = -224;
  static const int DESKTOP_SIZE = -223;

  static const int QUALITY_LEVEL_0 = -32;
  static const int QUALITY_LEVEL_1 = -31;
  static const int QUALITY_LEVEL_2 = -30;
  static const int QUALITY_LEVEL_3 = -29;
  static const int QUALITY_LEVEL_4 = -28;
  static const int QUALITY_LEVEL_5 = -27;
  static const int QUALITY_LEVEL_6 = -26;
  static const int QUALITY_LEVEL_7 = -25;
  static const int QUALITY_LEVEL_8 = -24;
  static const int QUALITY_LEVEL_9 = -23;

  static const char *const SIG_COMPR_LEVEL;
  static const char *const SIG_X_CURSOR;
  static const char *const SIG_RICH_CURSOR;
  static const char *const SIG_POINTER_POS;
  static const char *const SIG_LAST_RECT;
  static const char *const SIG_DESKTOP_SIZE;
  static const char *const SIG_QUALITY_LEVEL;
};

#endif // __RFB_ENCODING_DEFS_H_INCLUDED__
