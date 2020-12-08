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

#include "Encoder.h"

Encoder::Encoder(PixelConverter *conv, DataOutputStream *output)
: m_pixelConverter(conv),
  m_output(output)
{
}

Encoder::~Encoder(void)
{
}

int Encoder::getCode() const
{
  return EncodingDefs::RAW;
}

void Encoder::splitRectangle(const Rect *rect,
                             std::vector<Rect> *rectList,
                             const FrameBuffer *serverFb,
                             const EncodeOptions *options)
{
  rectList->push_back(*rect);
}

void Encoder::sendRectangle(const Rect *rect,
                            const FrameBuffer *serverFb,
                            const EncodeOptions *options)
{
  const FrameBuffer *fb = m_pixelConverter->convert(rect, serverFb);
  int pixelSize = (int)fb->getBytesPerPixel();
  _ASSERT(pixelSize == fb->getBytesPerPixel());

  UINT8 *buffer = (UINT8 *)fb->getBuffer();
  int lineWidth = rect->getWidth();
  int fbWidth = fb->getDimension().width;
  int lineSizeInBytes = lineWidth * pixelSize;
  int stride = fbWidth * pixelSize;
  UINT8 *lineP = &buffer[(rect->top * fbWidth + rect->left) * pixelSize];

  // Send the rectangle as is, line by line.
  for (int i = rect->top; i < rect->bottom; i++, lineP += stride) {
    m_output->writeFully((char *)lineP, lineSizeInBytes);
  }
}
