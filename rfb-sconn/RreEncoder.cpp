// Copyright (C) 2013 GlavSoft LLC.
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

#include "RreEncoder.h"

RreEncoder::RreEncoder(PixelConverter *conv, DataOutputStream *output)
: Encoder(conv, output)
{
  m_rects.reserve(4096);
}

RreEncoder::~RreEncoder()
{
}

int RreEncoder::getCode() const
{
  return EncodingDefs::RRE;
}

void RreEncoder::splitRectangle(const Rect *rect,
                                std::vector<Rect> *rectList,
                                const FrameBuffer *serverFb,
                                const EncodeOptions *options)
{
  for (int y0 = rect->top; y0 < rect->bottom; y0 += RECT_SIZE) {
    for (int x0 = rect->left; x0 < rect->right; x0 += RECT_SIZE) {
      int x1 = min(x0 + RECT_SIZE, rect->right);
      int y1 = min(y0 + RECT_SIZE, rect->bottom);
      rectList->push_back(Rect(x0, y0, x1, y1));
    }
  }
}

void RreEncoder::sendRectangle(const Rect *rect,
                               const FrameBuffer *serverFb,
                               const EncodeOptions *options)
{
  const FrameBuffer *fb = m_pixelConverter->convert(rect, serverFb);

  size_t bpp = fb->getBitsPerPixel();
  // Choose size of pixel according to options.
  if (bpp == 8) {
    rreEncode<UINT8>(rect, fb);
  } else if (bpp == 16) {
    rreEncode<UINT16>(rect, fb);
  } else if (bpp == 32) {
    rreEncode<UINT32>(rect, fb);
  } else {
    _ASSERT(0);
  }
}

template <class PIXEL_T>
void RreEncoder::rreEncode(const Rect *r,
                           const FrameBuffer *frameBuffer)
{
  PIXEL_T *buffer = (PIXEL_T *)frameBuffer->getBuffer();
  int fbWidth = frameBuffer->getDimension().width;
  PixelFormat pxFormat = frameBuffer->getPixelFormat();
  // Mask for cutting rubbish bits.
  PIXEL_T mask = pxFormat.redMax << pxFormat.redShift |
                 pxFormat.greenMax << pxFormat.greenShift |
                 pxFormat.blueMax << pxFormat.blueShift;
  
  PIXEL_T backgroundPixelValue = buffer[r->top * fbWidth + r->left] & mask;
  
  // Clear the cache with m_rects.
  m_rects.resize(0);

  vector<PIXEL_T> subrectPixelValue;

  // Find lines with the same pixel values.
  for (int i = r->top; i < r->bottom; i++) {
    for (int j = r->left; j < r->right; j++) {
      if ((buffer[i * fbWidth + j] & mask) != backgroundPixelValue) {
        if (subrectPixelValue.empty() ||
            (buffer[i * fbWidth + j] & mask) != (buffer[i * fbWidth + j - 1] & mask) ||
            m_rects.back().top != (i - r->top)) {
          subrectPixelValue.push_back(buffer[i * fbWidth + j] & mask);
          Rect rect(1, 1);
          rect.setLocation(j - r->left, i - r->top);
          m_rects.push_back(rect);
        } else {
          ++m_rects.back().right;
        }
      }
    }
  }
  
  // Send header.
  m_output->writeUInt32(static_cast<UINT32>(subrectPixelValue.size()));
  m_output->writeFully(&backgroundPixelValue, sizeof(PIXEL_T));
  
  // Send subrectangles.
  for (size_t i = 0; i < subrectPixelValue.size(); i++) {
    m_output->writeFully(&subrectPixelValue[i], sizeof(PIXEL_T));
    m_output->writeUInt16(m_rects[i].left);
    m_output->writeUInt16(m_rects[i].top);
    m_output->writeUInt16(m_rects[i].getWidth());
    m_output->writeUInt16(m_rects[i].getHeight());
  }
}
