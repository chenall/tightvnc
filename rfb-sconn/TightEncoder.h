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

#ifndef __RFB_TIGHT_ENCODER_H_INCLUDED__
#define __RFB_TIGHT_ENCODER_H_INCLUDED__

// FIXME: Use some object-oriented wrapper instead of the pure zlib.
#include "zlib/zlib.h"

#include "Encoder.h"
#include "TightPalette.h"
#include "JpegCompressor.h"

class TightEncoder : public Encoder
{
  friend class JpegEncoder;

public:
  TightEncoder(PixelConverter *conv, DataOutputStream *output);
  virtual ~TightEncoder();

  virtual int getCode() const;

  // Splits big rectangles according to the configuration setings (m_conf)
  // corresponding to the compression level set in EncodeOptions.
  virtual void splitRectangle(const Rect *rect,
                              std::vector<Rect> *rectList,
                              const FrameBuffer *serverFb,
                              const EncodeOptions *options);

  virtual void sendRectangle(const Rect *rect,
                             const FrameBuffer *serverFb,
                             const EncodeOptions *options) throw(IOException);

protected:
  // An implementation of sendRectangle() for the given pixel size.
  template <class PIXEL_T>
    void sendAnyRect(const Rect *rect,
                     const FrameBuffer *serverFb,
                     const FrameBuffer *clientFb,
                     const EncodeOptions *options) throw(IOException);

  // Send a solid-color rectangle.
  void sendSolidRect(const Rect *r, const FrameBuffer *fb) throw(IOException);

  // Send a two-color rectangle (1 bit per pixel).
  template <class PIXEL_T>
    void sendMonoRect(const Rect *rect,
                      const FrameBuffer *fb,
                      const EncodeOptions *options) throw(IOException);

  // Send an indexed-color rectangle (1 byte per pixel).
  template <class PIXEL_T>
    void sendIndexedRect(const Rect *rect,
                         const FrameBuffer *fb,
                         const EncodeOptions *options) throw(IOException);

  // Send a true color rectangle.
  template <class PIXEL_T>
    void sendFullColorRect(const Rect *rect,
                           const FrameBuffer *fb,
                           const EncodeOptions *options) throw(IOException);

  // Send a rectangle encoded with JPEG.
  void sendJpegRect(const Rect *rect,
                    const FrameBuffer *serverFb,
                    const EncodeOptions *options) throw(IOException);

  // Return true if 32-bit pixels should be packed into 24-bit representation,
  // false otherwise. This function should be given the client's pixel format.
  bool shouldPackPixels(const PixelFormat *pf) const;

  // Convert 32-bit color samples into 24-bit sequences, in place. It should
  // be called only when bpp is 32; color depth is 24; and redMax, greenMax
  // and blueMax are all 255.
  static void packPixels(UINT8 *buf, int count, const PixelFormat *pf);

  // Fill in the palette (m_pal) assuming that pixels have the type PIXEL_T
  // (where PIXEL_T can be UINT8, UINT16 or UINT32). Do not allow more than
  // maxColors in the palette, reset the palette size to 0 if actual number of
  // colors exceeds this limitation.
  template <class PIXEL_T>
    void fillPalette(const Rect *r, const FrameBuffer *fb, int maxColors);

  // Copy pixel data from the frame buffer to a byte array.
  template <class PIXEL_T>
    void copyPixels(const Rect *rect, const FrameBuffer *fb, UINT8 *dst);

  // Encode a two-color rectangle using m_pal as a palette, produce a bitmap
  // where one pixel is represented by one bit. Each line is padded with
  // zeroes to the byte boundary.
  // FIXME: Do not use DataOutputStream, do not throw IOException.
  template <class PIXEL_T>
    void encodeMonoRect(const Rect *rect, const FrameBuffer *fb,
                        DataOutputStream *out) throw(IOException);

  // Encode a rectangle using m_pal as a palette, produce a pixmap where one
  // pixel is represented by one byte which is its index in the palette.
  // FIXME: Do not use DataOutputStream, do not throw IOException.
  template <class PIXEL_T>
    void encodeIndexedRect(const Rect *rect, const FrameBuffer *fb,
                           DataOutputStream *out) throw(IOException);

  // FIXME: Throw ZlibException instead.
  void sendCompressed(const char *data, size_t dataLen,
                      int streamId, int zlibLevel) throw(IOException);

  // Send the number of the compressed bytes following. The number (dataLen)
  // is represented by a variable-length code (1..3 bytes).
  void sendCompactLength(size_t dataLen) throw(IOException);

  // Configuration table of the Tight encoder. Do not access this table
  // directly, use getConf() method instead.
  static const struct Conf {
    int maxRectSize;
    int maxRectWidth;
    int monoMinRectSize;
    int idxZlibLevel;
    int monoZlibLevel;
    int rawZlibLevel;
    int idxMaxColorsDivisor;
  } m_conf[10];

  // Select a record from the m_conf array which corresponds to the
  // compression level specified in the EncodeOptions object.
  static const Conf &getConf(const EncodeOptions *options);

  // Codes used in Tight-encoded data.
  static const UINT8 SUBENCODING_FILL = 0x80;
  static const UINT8 SUBENCODING_JPEG = 0x90;
  static const UINT8 EXPLICIT_FILTER = 0x40;
  static const UINT8 FILTER_PALETTE = 0x01;

  // Changing this will break compatibility with Tight decoders.
  static const int TIGHT_MIN_TO_COMPRESS = 12;

  // The parameters below may be adjusted.
  static const int DEFAULT_COMPRESSION_LEVEL = 6;
  static const int JPEG_MIN_RECT_SIZE = 4096;
  static const int JPEG_MIN_RECT_WIDTH = 8;
  static const int JPEG_MIN_RECT_HEIGHT = 8;

  // The number of zlib streams used by TightEncoder (it cannot exceed 4).
  static const int NUM_ZLIB_STREAMS = 3;

  // Indexes of individual zlib streams.
  static const int ZLIB_STREAM_RAW = 0;
  static const int ZLIB_STREAM_MONO = 1;
  static const int ZLIB_STREAM_IDX = 2;

  // The array of zlib stream structures.
  z_stream m_zsStruct[NUM_ZLIB_STREAMS];

  // The array of flags indicating if corresponding zlib streams were
  // initialized.
  bool m_zsActive[NUM_ZLIB_STREAMS];
  int m_zsLevel[NUM_ZLIB_STREAMS];

  // Color palette which maps color samples to color indexes and keeps track
  // of the number of colors allocated.
  TightPalette m_pal;

  // JPEG compressor working via the IJG JPEG library.
  StandardJpegCompressor m_compressor;
};

#endif // __RFB_TIGHT_ENCODER_H_INCLUDED__
