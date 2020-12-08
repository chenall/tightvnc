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

#ifndef _TIGHT_DECODER_H_
#define _TIGHT_DECODER_H_

#include <vector>

#include "util/Inflater.h"

#include "DecoderOfRectangle.h"
#include "JpegDecompressor.h"

class TightDecoder : public DecoderOfRectangle
{
public:
  TightDecoder(LogWriter *logWriter);
  virtual ~TightDecoder();

protected:
  virtual void decode(RfbInputGate *input,
                      FrameBuffer *frameBuffer,
                      const Rect *dstRect);

private:
  void reset();
  void resetDecoders(UINT8 compControl);
  UINT32 readTightPixel(RfbInputGate *input, int bytesPerCPixel);
  int readCompactSize(RfbInputGate *input);
  vector<UINT32> readPalette(RfbInputGate *input,
                          int paletteSize,
                          int bytesPerCPixel);
  void processJpeg(RfbInputGate *input,
                   FrameBuffer *frameBuffer,
                   const Rect *dstRect);
  void processBasicTypes(RfbInputGate *input,
                         FrameBuffer *frameBuffer,
                         const Rect *dstRect,
                         UINT8 compControl);
  void readTightData(RfbInputGate *input,
                     vector<UINT8> &buffer,
                     size_t expectedLength,
                     const int decoderId);
  void readCompressedData(RfbInputGate *input,
                          vector<UINT8> &buffer,
                          size_t expectedLength,
                          const int decoderId);
  void drawPalette(FrameBuffer *fb,
                   const vector<UINT32> &palette,
                   const vector<UINT8> &pixels,
                   const Rect *dstRect);
  void drawGradient(FrameBuffer *fb,
                    const vector<UINT8> &pixels,
                    const Rect *dstRect);
  void drawTightBytes(FrameBuffer *fb,
                     const vector<UINT8> *pixels,
                     const Rect *dstRect);
  void drawJpegBytes(FrameBuffer *fb,
                     const vector<UINT8> *pixels,
                     const Rect *dstRect);

  UINT32 getRawTightColor(const PixelFormat *pxFormat,
                          const vector<UINT8> &pixels,
                          size_t offset);
  void fillRawComponents(const PixelFormat *pxFormat,
                         UINT8 components[],
                         const vector<UINT8> &pixels,
                         size_t pixelOffset);

  UINT32 transformPixelToTight(UINT32 color);
  vector<UINT8> TightDecoder::transformArray(const vector<UINT8> &buffer);

  vector<Inflater *> m_inflater;
  JpegDecompressor m_jpeg;

  bool m_isCPixel;
private:
  static const int MAX_SUBENCODING = 0x09;
  static const int JPEG_TYPE = 0x09;
  static const int FILL_TYPE = 0x08;

  static const int FILTER_ID_MASK = 0x40;
  // TODO: removed 0x30 constant?
  static const int STREAM_ID_MASK = 0x30;

  static const int COPY_FILTER = 0x00;
  static const int PALETTE_FILTER = 0x01;
  static const int GRADIENT_FILTER = 0x02;

  static const int DECODERS_NUM = 4;

  static const int MIN_SIZE_TO_COMPRESS = 12;
};

#endif
