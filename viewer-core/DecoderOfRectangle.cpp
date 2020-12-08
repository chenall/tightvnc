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

#include "DecoderOfRectangle.h"

#include "FbUpdateNotifier.h"

DecoderOfRectangle::DecoderOfRectangle(LogWriter *logWriter)
: Decoder(logWriter)
{
}

DecoderOfRectangle::~DecoderOfRectangle()
{
}

void DecoderOfRectangle::process(RfbInputGate *input,
                     FrameBuffer *frameBuffer,
                     FrameBuffer *secondFrameBuffer,
                     const Rect *rect,
                     LocalMutex *fbLock,
                     FbUpdateNotifier *fbNotifier)
{
  decode(input, secondFrameBuffer, rect);
  copy(frameBuffer, secondFrameBuffer, rect, fbLock);
  notify(fbNotifier, rect);
}

void DecoderOfRectangle::copy(FrameBuffer *dstFrameBuffer,
                   const FrameBuffer *srcFrameBuffer,
                   const Rect *rect,
                   LocalMutex *fbLock)
{
  AutoLock al(fbLock);
  dstFrameBuffer->copyFrom(rect, srcFrameBuffer, rect->left, rect->top);
}

void DecoderOfRectangle::notify(FbUpdateNotifier *fbNotifier,
                     const Rect *rect)
{
  fbNotifier->onUpdate(rect);
}

bool DecoderOfRectangle::isPseudo() const
{
  return false;
}
