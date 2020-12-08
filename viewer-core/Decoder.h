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

#ifndef _DECODER_H_
#define _DECODER_H_

#include "log-writer/LogWriter.h"
#include "network/RfbInputGate.h"
#include "region/Rect.h"
#include "rfb/FrameBuffer.h"
#include "rfb/EncodingDefs.h"
#include "thread/AutoLock.h"
#include "thread/LocalMutex.h"

class Decoder
{
public:
  Decoder(LogWriter *logWriter);
  virtual ~Decoder();

  //
  // This method return encoding of this Decoder.
  //
  virtual int getCode() const;

  //
  // This method return true, if decoder responsible for pseudo encoding.
  //
  virtual bool isPseudo() const;

  //
  // This static method return true, if "encoding" is pseudo encoding.
  //
  static bool isPseudo(int encoding);

protected:
  LogWriter *m_logWriter;

  int m_encoding;
};

#endif
