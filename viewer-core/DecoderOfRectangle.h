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

#ifndef _DECODER_OF_RECTANGLE_H_
#define _DECODER_OF_RECTANGLE_H_

#include "Decoder.h"

class FbUpdateNotifier;

class DecoderOfRectangle : public Decoder
{
public:
  DecoderOfRectangle(LogWriter *logWriter);
  virtual ~DecoderOfRectangle();

  //
  // This function does the following:
  //   1. read update of dstRect from input
  //   2. decode rectangle on "secondFrameBuffer"
  //   3. copy rectangle from secondFrameBuffer to "frameBuffer"
  //   4. notify fbNotifier
  // His called decode(), copy() and notify() by order, defined in implementation.
  //
  // This function is thread-safe for frameBuffer.
  //
  virtual void process(RfbInputGate *input,
                       FrameBuffer *frameBuffer,
                       FrameBuffer *secondFrameBuffer,
                       const Rect *rect,
                       LocalMutex *fbLock,
                       FbUpdateNotifier *fbNotifier);

  //
  // This method inherited Decoder::isPseudo() and return true.
  //
  virtual bool isPseudo() const;

protected:
  //
  // This method read rectangle-update from input and decode on frameBuffer.
  //
  virtual void decode(RfbInputGate *input,
                      FrameBuffer *frameBuffer,
                      const Rect *rect) = 0;

  //
  // This method copy rectangle from srcFrameBuffer to dstFrameBuffer.
  // This function is thread-safe from dstFrameBuffer.
  //
  virtual void copy(FrameBuffer *dstFrameBuffer,
                    const FrameBuffer *srcFrameBuffer,
                    const Rect *rect,
                    LocalMutex *fbLock);

  //
  // This method notify fbNotifier about update of rect.
  //
  virtual void notify(FbUpdateNotifier *fbNotifier,
                      const Rect *rect);
};

#endif
