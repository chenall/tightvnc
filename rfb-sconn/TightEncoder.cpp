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

#include "TightEncoder.h"

#include "io-lib/ByteArrayOutputStream.h"

TightEncoder::TightEncoder(PixelConverter *conv, DataOutputStream *output)
: Encoder(conv, output)
{
  for (int i = 0; i < NUM_ZLIB_STREAMS; i++) {
    m_zsActive[i] = false;
  }
}

TightEncoder::~TightEncoder()
{
  for (int i = 0; i < NUM_ZLIB_STREAMS; i++) {
    if (m_zsActive[i]) {
      deflateEnd(&m_zsStruct[i]);
    }
  }
}

int TightEncoder::getCode() const
{
  return EncodingDefs::TIGHT;
}

void TightEncoder::splitRectangle(const Rect *rect,
                                  std::vector<Rect> *rectList,
                                  const FrameBuffer *serverFb,
                                  const EncodeOptions *options)
{
  int maxSize = getConf(options).maxRectSize;
  int rectWidth = rect->getWidth();

  // Handle a special case -- don't ever split a rectangle while its area does
  // not exceed 1/4 of maximum allowed area and width does not exceed 2048.
  if (rectWidth <= 2048 && rect->area() <= maxSize / 4) {
    rectList->push_back(rect);
    return;
  }

  int maxWidth = getConf(options).maxRectWidth;
  if (maxWidth > rectWidth) {
    maxWidth = rectWidth;
  }
  
  int maxHeight = maxSize / maxWidth;

  for (int y0 = rect->top; y0 < rect->bottom; y0 += maxHeight) {
    int y1 = min(y0 + maxHeight, rect->bottom);
    for (int x0 = rect->left; x0 < rect->right; x0 += maxWidth) {
      int x1 = min(x0 + maxWidth, rect->right);
      rectList->push_back(Rect(x0, y0, x1, y1));
    }
  }
}

void TightEncoder::sendRectangle(const Rect *rect,
                                 const FrameBuffer *serverFb,
                                 const EncodeOptions *options)
{
  // First, convert pixels to client format.
  const FrameBuffer *clientFb = m_pixelConverter->convert(rect, serverFb);

  // Now call an encoder function corresponding to the client's pixel size.
  size_t bpp = clientFb->getBitsPerPixel();
  switch (bpp) {
  case 8:
    sendAnyRect<UINT8>(rect, serverFb, clientFb, options);
    break;
  case 16:
    sendAnyRect<UINT16>(rect, serverFb, clientFb, options);
    break;
  case 32:
    sendAnyRect<UINT32>(rect, serverFb, clientFb, options);
    break;
  default:
    _ASSERT(0);
  }
}

//--------------------------------------------------------------------------//

// FIXME: Is it really necessary to pass both frame buffers in arguments?
// FIXME: Make a special version for the case when PIXEL_T is UINT8.
template <class PIXEL_T>
void TightEncoder::sendAnyRect(const Rect *rect,
                               const FrameBuffer *serverFb,
                               const FrameBuffer *clientFb,
                               const EncodeOptions *options)
{
  // Compute maximum number of colors to be allowed in the palette.
  int maxColors = rect->area() / getConf(options).idxMaxColorsDivisor;
  if (maxColors < 2) {
    if (rect->area() >= getConf(options).monoMinRectSize) {
      maxColors = 2;
    } else {
      // Always allow at least one color in the palette.
      maxColors = 1;
    }
  }

  // Fill in the palette (m_pal).
  fillPalette<PIXEL_T>(rect, clientFb, maxColors);

  // If that was a solid-color rectangle, sent it.
  int numColors = m_pal.getNumColors();
  if (numColors == 1) {
    sendSolidRect(rect, clientFb);
  } else if (numColors == 2) {
    sendMonoRect<PIXEL_T>(rect, clientFb, options);
  } else if (sizeof(PIXEL_T) > 1 && numColors != 0) {
    sendIndexedRect<PIXEL_T>(rect, clientFb, options);
  } else if (sizeof(PIXEL_T) > 1 &&
             options->jpegEnabled() &&
             serverFb->getBitsPerPixel() >= 16 &&
             rect->area() >= JPEG_MIN_RECT_SIZE &&
             rect->getWidth() >= JPEG_MIN_RECT_WIDTH &&
             rect->getHeight() >= JPEG_MIN_RECT_HEIGHT) {
    sendJpegRect(rect, serverFb, options);
  } else {
    sendFullColorRect<PIXEL_T>(rect, clientFb, options);
  }
}

void TightEncoder::sendSolidRect(const Rect *r, const FrameBuffer *fb)
{
  PixelFormat pf = fb->getPixelFormat();
  size_t pixelSize = pf.bitsPerPixel / 8;

  // Copy the leftmost upper pixel of the rectangle into a buffer.
  UINT8 buf[4];
  memcpy(buf, fb->getBufferPtr(r->left, r->top), pixelSize);
  // FIXME: Call packPixels() unconditionally, make it return length in bytes?
  if (shouldPackPixels(&pf)) {
    packPixels(buf, 1, &pf);
    pixelSize = 3;
  }

  m_output->writeUInt8(SUBENCODING_FILL);
  m_output->writeFully(buf, pixelSize);
}

template <class PIXEL_T>
void TightEncoder::sendMonoRect(const Rect *rect,
                                const FrameBuffer *fb,
                                const EncodeOptions *options)
{
  // Send control info.
  const int zlibStreamId = ZLIB_STREAM_MONO;
  m_output->writeUInt8(EXPLICIT_FILTER | zlibStreamId << 4);
  m_output->writeUInt8(FILTER_PALETTE);
  m_output->writeUInt8(1); // the number of colors minus 1

  // Prepare output buffer.
  int dataLen = (rect->getWidth() + 7) / 8;
  dataLen *= rect->getHeight();
  // FIXME: Optimize, use char[] instead of ByteArrayOutputStream.
  ByteArrayOutputStream encoded(dataLen);
  DataOutputStream encodedData(&encoded);

  // Send the palette.
  PIXEL_T palette[2] = {
    (PIXEL_T)m_pal.getEntry(0),
    (PIXEL_T)m_pal.getEntry(1)
  };
  PixelFormat pf = fb->getPixelFormat();
  size_t pixelSize = sizeof(PIXEL_T);
  if (shouldPackPixels(&pf)) {
    packPixels((UINT8 *)palette, 2, &pf);
    pixelSize = 3;
  }
  m_output->writeFully(palette, pixelSize * 2);

  // Convert image to indexed colors.
  encodeMonoRect<PIXEL_T>(rect, fb, &encodedData);
  _ASSERT(encoded.size() == dataLen);

  // Compress and send.
  int zlibLevel = getConf(options).monoZlibLevel;
  sendCompressed(encoded.toByteArray(), dataLen, zlibStreamId, zlibLevel);
}

template <class PIXEL_T>
void TightEncoder::sendIndexedRect(const Rect *rect,
                                   const FrameBuffer *fb,
                                   const EncodeOptions *options)
{
  // Send control info.
  const int zlibStreamId = ZLIB_STREAM_IDX;
  m_output->writeUInt8(EXPLICIT_FILTER | zlibStreamId << 4);
  m_output->writeUInt8(FILTER_PALETTE);
  int numColors = m_pal.getNumColors();
  m_output->writeUInt8((UINT8)(numColors - 1));

  // Prepare output buffer.
  int dataLen = rect->getWidth() * rect->getHeight();
  // FIXME: Optimize, use char[] instead of ByteArrayOutputStream.
  ByteArrayOutputStream encoded(dataLen);
  DataOutputStream encodedData(&encoded);

  // Send the palette.
  PIXEL_T palette[256];
  for (int i = 0; i < numColors; i++) {
    palette[i] = (PIXEL_T)m_pal.getEntry(i);
  }
  PixelFormat pf = fb->getPixelFormat();
  size_t pixelSize = sizeof(PIXEL_T);
  if (shouldPackPixels(&pf)) {
    packPixels((UINT8 *)palette, numColors, &pf);
    pixelSize = 3;
  }
  m_output->writeFully(palette, pixelSize * numColors);

  // Convert image to indexed colors.
  encodeIndexedRect<PIXEL_T>(rect, fb, &encodedData);
  _ASSERT(encoded.size() == dataLen);

  // Compress and send.
  int zlibLevel = getConf(options).idxZlibLevel;
  sendCompressed(encoded.toByteArray(), dataLen, zlibStreamId, zlibLevel);
}

template <class PIXEL_T>
void TightEncoder::sendFullColorRect(const Rect *rect,
                                     const FrameBuffer *fb,
                                     const EncodeOptions *options)
{
  // Send control info.
  const int zlibStreamId = ZLIB_STREAM_RAW;
  m_output->writeUInt8(zlibStreamId << 4);

  // Prepare output buffer.
  int dataLen = rect->area() * sizeof(PIXEL_T);
  // FIXME: Use char[] instead?
  std::vector<UINT8> rgbData(dataLen);

  // Get pixels from the frame buffer.
  copyPixels<PIXEL_T>(rect, fb, &rgbData.front());
  _ASSERT(rgbData.size() == dataLen);

  // Pack pixels into 24-bit samples if necessary.
  PixelFormat pf = fb->getPixelFormat();
  if (shouldPackPixels(&pf)) {
    packPixels(&rgbData.front(), rect->area(), &pf);
    rgbData.resize(rect->area() * 3);
  }

  // Compress and send.
  int zlibLevel = getConf(options).rawZlibLevel;
  // FIXME: Get rid of explicit conversions between chars and bytes.
  sendCompressed((const char *)&rgbData.front(), rgbData.size(),
                 zlibStreamId, zlibLevel);
}

void TightEncoder::sendJpegRect(const Rect *rect,
                                const FrameBuffer *serverFb,
                                const EncodeOptions *options)
{
  _ASSERT(options->jpegEnabled());

  // Set proper JPEG quality level in the compressor. The default value 6
  // below does not mean anything, it will not be used because we assume
  // valid JPEG quality level was set in the options object.
  int quality = options->getJpegQualityLevel(6);
  m_compressor.setQuality(quality * 10 + 5);

  // Shortcuts.
  const void *ptr = serverFb->getBufferPtr(rect->left, rect->top);
  PixelFormat fmt = serverFb->getPixelFormat();
  int width = rect->getWidth();
  int height = rect->getHeight();
  int stride = serverFb->getBytesPerRow();

  // Compress pixels.
  m_compressor.compress(ptr, &fmt, width, height, stride);
  size_t dataLength = m_compressor.getOutputLength();

  // Actually send the encoded data.
  m_output->writeUInt8(SUBENCODING_JPEG);
  sendCompactLength(dataLength);
  m_output->writeFully(m_compressor.getOutputData(), dataLength);
}

//--------------------------------------------------------------------------//

bool TightEncoder::shouldPackPixels(const PixelFormat *pf) const
{
  return (pf->colorDepth == 24 &&
          pf->redMax == 0xFF &&
          pf->greenMax == 0xFF &&
          pf->blueMax == 0xFF &&
          pf->bitsPerPixel == 32);
}

void TightEncoder::packPixels(UINT8 *buf, int count, const PixelFormat *pf)
{
  UINT8 *dst = buf;
  UINT32 pix;

  while (count--) {
    if (!pf->bigEndian) {
      pix = (UINT32)buf[3] << 24 |
            (UINT32)buf[2] << 16 |
            (UINT32)buf[1] << 8 |
            (UINT32)buf[0];
    } else {
      pix = (UINT32)buf[0] << 24 |
            (UINT32)buf[1] << 16 |
            (UINT32)buf[2] << 8 |
            (UINT32)buf[3];
    }
    buf += 4;
    *dst++ = (UINT8)(pix >> pf->redShift);
    *dst++ = (UINT8)(pix >> pf->greenShift);
    *dst++ = (UINT8)(pix >> pf->blueShift);
  }
}

template <class PIXEL_T>
void TightEncoder::fillPalette(const Rect *r, const FrameBuffer *fb, int maxColors)
{
  // Clear the palette.
  m_pal.reset();
  m_pal.setMaxColors(maxColors);

  // Shortcuts.
  const PIXEL_T *pixels = (const PIXEL_T *)fb->getBuffer();
  int stride = fb->getDimension().width;
  UINT32 pixel = pixels[r->top * stride + r->left];
  UINT32 oldPixel = 0;
  UINT32 runLength = 0;

  for (int y = r->top; y < r->bottom; y++) {
    for (int x = r->left; x < r->right; x++) {
      pixel = pixels[y * stride + x];

      if (oldPixel != pixel) {
        if (m_pal.insert(oldPixel, runLength) == 0) {
          return;
        }
        oldPixel = pixel;
        runLength = 1;
      } else {
        runLength++;
      }
    }
  }
  if (m_pal.insert(oldPixel, runLength) == 0) {
			return;
	}
}

template <class PIXEL_T>
void TightEncoder::copyPixels(const Rect *rect, const FrameBuffer *fb,
                              UINT8 *dst)
{
  const int rectWidth = rect->getWidth();
  const int rectHeight = rect->getHeight();

  const PIXEL_T *src = (const PIXEL_T *)fb->getBufferPtr(rect->left, rect->top);
  const int fbStride = fb->getDimension().width;
  const int bytesPerRow = rectWidth * sizeof(PIXEL_T);

  for (int y = 0; y < rectHeight; y++) {
    memcpy(dst, src, bytesPerRow);
    src += fbStride;
    dst += bytesPerRow;
  }
}

template <class PIXEL_T>
void TightEncoder::encodeMonoRect(const Rect *rect, const FrameBuffer *fb,
                                  DataOutputStream *out)
{
  const PIXEL_T *src = (const PIXEL_T *)fb->getBufferPtr(rect->left, rect->top);
  const int w = rect->getWidth();
  const int h = rect->getHeight();
  const PIXEL_T bg = (PIXEL_T)m_pal.getEntry(0);

  unsigned int value, mask;
  int x, y, bits;
  const int alignedWidth = w - w % 8;
  const int skipPixels = fb->getDimension().width - w;

  for (y = 0; y < h; y++) {
    for (x = 0; x < alignedWidth; x += 8) {
      for (bits = 0; bits < 8; bits++) {
        if (*src++ != bg)
          break;
      }
      if (bits == 8) {
        out->writeUInt8(0);
        continue;
      }
      mask = 0x80 >> bits;
      value = mask;
      for (bits++; bits < 8; bits++) {
        mask >>= 1;
        if (*src++ != bg) {
          value |= mask;
        }
      }
      out->writeUInt8((UINT8)value);
    }
    if (x < w) {
      mask = 0x80;
      value = 0;
      do {
        if (*src++ != bg) {
          value |= mask;
        }
        mask >>= 1;
      } while (++x < w);
      out->writeUInt8((UINT8)value);
    }
    src += skipPixels;
  }
}

template <class PIXEL_T>
void TightEncoder::encodeIndexedRect(const Rect *rect, const FrameBuffer *fb,
                                     DataOutputStream *out)
{
  const PIXEL_T *src = (const PIXEL_T *)fb->getBufferPtr(rect->left, rect->top);
  const int w = rect->getWidth();
  const int h = rect->getHeight();
  const int skipPixels = fb->getDimension().width - w;

  UINT8 index = m_pal.getIndex(*src);
  PIXEL_T oldColor = 0;
  for (int y = 0; y < h; y++) {
    for (int x = 0; x < w; x++) {
      if (oldColor != *src) {
        index = m_pal.getIndex(*src);
        oldColor = *src;
      }
      *src++;
      out->writeUInt8(index);
    }
    src += skipPixels;
  }
}

void TightEncoder::sendCompressed(const char *data, size_t dataLen,
                                  int streamId, int zlibLevel)
{
  if (dataLen < TIGHT_MIN_TO_COMPRESS) {
    m_output->writeFully(data, dataLen);
    return;
  }

  z_streamp pz = &m_zsStruct[streamId];

  // Initialize compression stream if needed.
  if (!m_zsActive[streamId]) {
    pz->zalloc = Z_NULL;
    pz->zfree = Z_NULL;
    pz->opaque = Z_NULL;

    int err = deflateInit2(pz, zlibLevel, Z_DEFLATED, MAX_WBITS,
                           MAX_MEM_LEVEL, Z_DEFAULT_STRATEGY);
    if (err != Z_OK) {
      throw IOException(_T("Zlib stream initialization failed in Tight encoder"));
    }

    m_zsActive[streamId] = true;
    m_zsLevel[streamId] = zlibLevel;
  }

  // Prepare buffers.
  size_t compressedBufferSize = dataLen + dataLen / 100 + 16;

  std::vector<char> charBuff(compressedBufferSize);
  char *compressedData = &charBuff.front();

  _ASSERT((unsigned int)dataLen == dataLen);
  _ASSERT((unsigned int)compressedBufferSize == compressedBufferSize);
  pz->next_in = (Bytef *)data;
  pz->avail_in = (unsigned int)dataLen;
  pz->next_out = (Bytef *)compressedData;
  pz->avail_out = (unsigned int)compressedBufferSize;

  // Change compression parameters if needed.
  if (zlibLevel != m_zsLevel[streamId]) {
    int err = deflateParams(pz, zlibLevel, Z_DEFAULT_STRATEGY);
    if (err != Z_OK) {
      throw IOException(_T("Error configuring Zlib stream in Tight encoder"));
    }
    m_zsLevel[streamId] = zlibLevel;
  }

  // Actual compression.
  int err = deflate(pz, Z_SYNC_FLUSH);
  if (err != Z_OK || pz->avail_in != 0 || pz->avail_out == 0) {
      throw IOException(_T("Zlib compression failed in Tight encoder"));
  }

  try {
    size_t compressedLength = compressedBufferSize - pz->avail_out;
    sendCompactLength(compressedLength);
    m_output->writeFully(compressedData, compressedLength);
  } catch (...) {
    throw;
  }
}

void TightEncoder::sendCompactLength(size_t dataLen)
{
  _ASSERT(dataLen <= 0x3FFFFF);

  UINT8 buffer[4];
  size_t numBytes = 0;

  buffer[numBytes++] = dataLen & 0x7F;
  if (dataLen > 0x7F) {
    buffer[numBytes-1] |= 0x80;
    buffer[numBytes++] = dataLen >> 7 & 0x7F;
    if (dataLen > 0x3FFF) {
      buffer[numBytes-1] |= 0x80;
      buffer[numBytes++] = dataLen >> 14 & 0xFF;
    }
  }

  m_output->writeFully(buffer, numBytes);
}

// FIXME: Values for maxRectSize and maxRectWidth should be determined after
//        running TightEncoder on recorded test sessions. Current values were
//        intentionally made small because we do not implement algorithms to
//        detect areas to be compressed with JPEG yet and thus we would like
//        to divide areas to avoid compressing too much with JPEG.
const TightEncoder::Conf TightEncoder::m_conf[10] = {
  {   512,   32,   6, 0, 0, 0,  4 },
  {  2048,   64,   6, 1, 1, 1,  8 },
  {  6144,  128,   8, 3, 3, 2, 24 },
  {  8192,  128,  12, 5, 5, 3, 32 },
  {  8192,  128,  12, 6, 6, 4, 32 },
  {  8192,  128,  12, 7, 7, 5, 32 },
  {  8192,  128,  16, 7, 7, 6, 48 },
  { 16384,  256,  16, 8, 8, 7, 64 },
  { 16384,  256,  32, 9, 9, 8, 64 },
  { 32768,  256,  32, 9, 9, 9, 96 }
};

const TightEncoder::Conf &
TightEncoder::getConf(const EncodeOptions *options)
{
  int level = options->getCompressionLevel(DEFAULT_COMPRESSION_LEVEL);
  _ASSERT(level >= 0 && level <= 9);
  return m_conf[level];
}
