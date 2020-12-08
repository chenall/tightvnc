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

#ifndef _DECODER_STORE_H_
#define _DECODER_STORE_H_

#include <map>
#include <vector>

#include "Decoder.h"

#include "log-writer/LogWriter.h"

class DecoderStore
{
public:
  DecoderStore(LogWriter *logWriter);
  ~DecoderStore();

  Decoder *getDecoder(INT32 decoderId);
  vector<INT32> getDecoderIds();
  
  // return true, if adding is complete
  // return false, if decoder already exist
  bool addDecoder(Decoder *newDecoder, int priority);
  // return true, if deleting is complete
  // return false, if decoder not exist
  bool removeDecoder(INT32 decoderId);

  void setPreferredEncoding(INT32 encodingType);
  void allowCopyRect(bool allow);

private:
  LogWriter *m_logWriter;

  map<INT32, pair<int, Decoder*> > m_decoders;
  INT32 m_preferredEncoding;
  bool m_allowCopyRect;
};

#endif
