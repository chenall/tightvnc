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

#include "ZrleEncoder.h"

ZrleEncoder::ZrleEncoder(PixelConverter *conv, DataOutputStream *output)
: Encoder(conv, output),
  // FIXME: This values (zlib options) is not used now.
  // May be to improve Deflater class?
  // FIXME: To make some experiments with other zlib values in the future.
  m_idxZlibLevel(ZLIB_IDX_LEVEL_DEFAULT),
  m_monoZlibLevel(ZLIB_MONO_LEVEL_DEFAULT),
  m_rawZlibLevel(ZLIB_RAW_LEVEL_DEFAULT),
  m_bytesPerPixel(0),
  m_numberFirstByte(0)
{
}

ZrleEncoder::~ZrleEncoder()
{
}

int ZrleEncoder::getCode() const
{
  return EncodingDefs::ZRLE;
}

void ZrleEncoder::splitRectangle(const Rect *rect,
                                 std::vector<Rect> *rectList,
                                 const FrameBuffer *serverFb,
                                 const EncodeOptions *options)
{
  rectList->push_back(*rect);
}

void ZrleEncoder::sendRectangle(const Rect *rect,
                                const FrameBuffer *serverFb,
                                const EncodeOptions *options)
{
  // Determing the number of bytes per pixel and the first byte of them.
  // It is possible only if red, green and blue intensities fit
  // in either the least significant or the most significant 3 bytes.
  // Used for futher work with CPIXELs.
  m_bytesPerPixel = 0;
  m_numberFirstByte = 0;
  const FrameBuffer *clientFb = m_pixelConverter->convert(rect, serverFb);
  //client pixel format
  m_pxFormat = clientFb->getPixelFormat();
  //server pixel format
  PixelFormat serverPxFormat = serverFb->getPixelFormat();
  bool bigEndianDiffs = m_pxFormat.bigEndian != serverPxFormat.bigEndian;
  if (m_pxFormat.bitsPerPixel == 8) {
    m_bytesPerPixel = 1;
  } else if (m_pxFormat.bitsPerPixel == 16) {
    m_bytesPerPixel = 2;
  } else if (m_pxFormat.bitsPerPixel == 32) {
    UINT32 colorMaxValue =  m_pxFormat.blueMax  << m_pxFormat.blueShift  |
                            m_pxFormat.greenMax << m_pxFormat.greenShift |
                            m_pxFormat.redMax   << m_pxFormat.redShift;
    //from big-endian to local
    if (bigEndianDiffs) {
      colorMaxValue = ntohl(colorMaxValue);
    }
    //for CPIXELS
    if ((colorMaxValue & (0xFF000000))==0) {
      m_bytesPerPixel = 3;
      m_numberFirstByte = 0;
    } else if ((colorMaxValue & 0xFF)==0) {
      m_bytesPerPixel = 3;
      m_numberFirstByte = 1;
    } else /*for other cases*/{
      m_bytesPerPixel = 4;
      m_numberFirstByte = 0;
    }
  }
 
  // Reserve data once for potentional transmitting of whole frame buffer
  // in raw encoding with CPIXELs.
  // If vector will be small it will be resized automatically.
  m_rgbData.reserve(rect->area() * 3);
  
  m_fbWidth = clientFb->getDimension().width;
  size_t bpp = clientFb->getBitsPerPixel();
  if (bpp == 8) {
    sendRect<UINT8>(rect, serverFb, clientFb, options);
  } else if (bpp == 16) {
    sendRect<UINT16>(rect, serverFb, clientFb, options);
  } else if (bpp == 32) {
    sendRect<UINT32>(rect, serverFb, clientFb, options);
  } else {
    _ASSERT(0);
  }
}

template <class PIXEL_T>
void ZrleEncoder::sendRect(const Rect *rect,
                           const FrameBuffer *serverFb,
                           const FrameBuffer *clientFb,
                           const EncodeOptions *options)
{
  m_rgbData.resize(0);
  const PIXEL_T *buffer = static_cast<const PIXEL_T *>(clientFb->getBuffer());
  
  Rect tileRect;
  for (tileRect.top = rect->top; tileRect.top < rect->bottom; tileRect.top += TILE_SIZE) {

    tileRect.bottom = min(rect->bottom, tileRect.top + TILE_SIZE);

    for (tileRect.left = rect->left; tileRect.left < rect->right; tileRect.left += TILE_SIZE) {

      tileRect.right = min(rect->right, tileRect.left + TILE_SIZE);

      // Clear sizes and vector with plain RLE tile.
      m_rawTileSize = 0;
      m_paletteTileSize = 0;
      m_paletteRleTileSize = 0;
      m_plainRleTile.clear();

      fillPalette<PIXEL_T>(&tileRect, clientFb);
      int numColors = m_pal.getNumColors();
      m_oldSize = m_rgbData.size();
      
      // If number of colors is 1 the tile with minimal size is solid.
      if (numColors == 1) {
        writeSolidTile();
      // Else calculate sizes of tile with other encodings
      // and choose encoding type when size is the minimal.
      } else {
        // Calculate size of packed pixels in palette.
        if (numColors == 2) {
          m_mSize = ((tileRect.getWidth() + 7) / 8) * tileRect.getHeight();
        } else if (numColors == 3 || numColors == 4) {
          m_mSize = ((tileRect.getWidth() + 3) / 4) * tileRect.getHeight();
        } else {
          m_mSize = ((tileRect.getWidth() + 1) / 2) * tileRect.getHeight();
        }
        
        //TODO: Test this code
        // Size of raw tile is (1 + width * height * pixelSize).
        m_rawTileSize = 1 + tileRect.area() * m_bytesPerPixel;
        // Size of palette tile.
        if (numColors > 1 && numColors <= 16) {
          m_paletteTileSize = 1 + numColors * m_bytesPerPixel + m_mSize;
        } else {
          m_paletteTileSize = THIS_TYPE_OF_TILE_IS_NOT_POSSIBLE;
        }
        // Size of palette RLE tile.
        if (numColors > 16 && numColors <= 127) {
          m_paletteRleTileSize += numColors * m_bytesPerPixel;
        } else {
          m_paletteRleTileSize = THIS_TYPE_OF_TILE_IS_NOT_POSSIBLE;
        }
        // Choose the size of the min tile.
        size_t minSizeOfTile = m_rawTileSize;
        if (m_paletteTileSize < minSizeOfTile) {
          minSizeOfTile = m_paletteTileSize;
        }
        if (m_plainRleTile.size() < minSizeOfTile) {
          minSizeOfTile = m_plainRleTile.size();
        }
        if (m_paletteRleTileSize < minSizeOfTile) {
          minSizeOfTile = m_paletteRleTileSize;
        }

        // Write the tile with the min size.
        if (minSizeOfTile == m_rawTileSize) {
          writeRawTile<PIXEL_T>(&tileRect, clientFb);
        } else if (minSizeOfTile == m_paletteTileSize) {
          writePackedPaletteTile<PIXEL_T>(&tileRect, clientFb);
        } else if (minSizeOfTile == m_plainRleTile.size()) {
          m_rgbData.resize(m_oldSize + m_plainRleTile.size());
          memcpy(&m_rgbData[m_oldSize],
                 &m_plainRleTile.front(),
                 m_plainRleTile.size());
        } else if (minSizeOfTile == m_paletteRleTileSize) {
          writePaletteRleTile<PIXEL_T>(&tileRect, clientFb);
        }
      }
    }
  }

  // If area of rect == 0, send length of zlib data == 0.
  if (m_rgbData.empty()) {
    m_output->writeUInt32(0);
  } else {
    m_deflater.setInput(reinterpret_cast<const char *>(&m_rgbData.front()),
                        m_rgbData.size());
    m_deflater.deflate();
  
    m_output->writeUInt32(m_deflater.getOutputSize());
    m_output->writeFully(m_deflater.getOutput(),
                         m_deflater.getOutputSize());
  }
}

template <class PIXEL_T>
void ZrleEncoder::writeRawTile(const Rect *tileRect,
                               const FrameBuffer *fb)
{
  m_oldSize = m_rgbData.size();
  m_rgbData.resize(m_oldSize + tileRect->area() * m_bytesPerPixel + 1);
  m_rgbData[m_oldSize] = 0;
  if (m_bytesPerPixel == 3) {
    copyCPixels(tileRect, fb, &m_rgbData[m_oldSize + 1]);
  } else {
    copyPixels<PIXEL_T>(tileRect, fb, &m_rgbData[m_oldSize + 1]);
  }
}

void ZrleEncoder::writeSolidTile() throw(IOException)
{
  m_oldSize = m_rgbData.size();
  UINT32 colorPixel = m_pal.getEntry(0);
  m_rgbData.resize(m_oldSize + m_bytesPerPixel + 1);
  m_rgbData[m_oldSize] = 1;
  memcpy(&m_rgbData[m_oldSize + 1], &colorPixel + m_numberFirstByte, m_bytesPerPixel);
}

template <class PIXEL_T>
void ZrleEncoder::writePackedPaletteTile(const Rect *tileRect,
                                         const FrameBuffer *fb)
{
  int numColors = m_pal.getNumColors();
  m_oldSize = m_rgbData.size();
  UINT8 deltaOffset;
  if (numColors == 2) {
    deltaOffset = 1;
  } else if (numColors == 3 || numColors == 4) {
    deltaOffset = 2;
  } else {
    deltaOffset = 4;
  }

  // Resize of m_rgbData for a new chunk of data.
  // m_oldSize + sizeof(subencodingByte + palette + packedPixels)
  m_rgbData.resize(m_oldSize + 1 + numColors * m_bytesPerPixel + m_mSize);

  // Write type of subencoding.
  m_rgbData[m_oldSize] = numColors;

  // Write palette.
  for (int i = 0; i < numColors; i++) {
    UINT32 buf = m_pal.getEntry(i);
    memcpy(&m_rgbData[m_oldSize + 1 + i * m_bytesPerPixel],
             &buf + m_numberFirstByte,
             m_bytesPerPixel);
  }

  // Pack pixels.
  const PIXEL_T *buffer = static_cast<const PIXEL_T *>(fb->getBuffer());
  UINT8 packedByte = 0;
  int indexOfM = 0;
  int offset = 8;

  Rect rect;
  for (rect.top = tileRect->top; rect.top < tileRect->bottom; rect.top++) {
    for (rect.left = tileRect->left; rect.left < tileRect->right; rect.left++) {
      PIXEL_T px = buffer[rect.top * m_fbWidth + rect.left];
      UINT8 indexOfColor = m_pal.getIndex(px);
      if (offset != 0) {
        packedByte = packedByte << deltaOffset;
        packedByte = packedByte | indexOfColor;
        offset -= deltaOffset;
      }
      else {
        // Write next packed byte.
        m_rgbData[m_oldSize + 1 + numColors * m_bytesPerPixel + indexOfM] = packedByte;
        indexOfM++;
        packedByte = 0;
        offset = 8;

        packedByte = packedByte << deltaOffset;
        packedByte = packedByte | indexOfColor;
        offset -= deltaOffset;
      }
    }
    while (offset != 0) {
      packedByte = packedByte << deltaOffset;
      offset -= deltaOffset;
    }
    // Write next packed byte.
    m_rgbData[m_oldSize + 1 + numColors * m_bytesPerPixel + indexOfM] = packedByte;
    indexOfM++;
    packedByte = 0;
    offset = 8;
  }
}

void ZrleEncoder::pushRunLengthPaletteRle(int runLength,
                                          std::vector<UINT8> *paletteRleData)
{
  do {
    if (runLength > 255) {
      paletteRleData->push_back(255); 
    } else {
      paletteRleData->push_back(runLength);
    }
    runLength -= 255;
  } while (runLength >= 0);
}

template <class PIXEL_T>
void ZrleEncoder::writePaletteRleTile(const Rect *tileRect,
                                      const FrameBuffer *fb)
{
  int numColors = m_pal.getNumColors();
  std::vector<UINT8> paletteRleData;
  paletteRleData.resize(1 + numColors * m_bytesPerPixel);

  // Write type of subencoding.
  paletteRleData[0] = numColors + 128;

  // Write palette.
  for (int i = 0; i < numColors; i++) {
    UINT32 buf = m_pal.getEntry(i);
    memcpy(&paletteRleData[1 + i * m_bytesPerPixel],
             &buf + m_numberFirstByte,
             m_bytesPerPixel);
  }

  const PIXEL_T *buffer = static_cast<const PIXEL_T *>(fb->getBuffer());
  PixelFormat pxFormat = fb->getPixelFormat();

  // There is the first iteration of loop below.
  PIXEL_T px = buffer[tileRect->top * m_fbWidth + tileRect->left];
  UINT8 indexOfColor = m_pal.getIndex(px);

  // Processing of the first pixel.
  paletteRleData.push_back(indexOfColor);
  UINT8 previousIndexOfColor = indexOfColor;
  
  int runLength = 0;
  for (int i = 1; i < tileRect->area(); ++i) {
    // FIXME: This variant may be not the most optimal.
    // One of the possible variant is double for loops.
    int x = tileRect->left + i % tileRect->getWidth();
    int y = tileRect->top + i / tileRect->getWidth();

    px = buffer[y * m_fbWidth + x];

    indexOfColor = m_pal.getIndex(px);
    if (indexOfColor != previousIndexOfColor) {
      if (runLength > 0) {
        pushRunLengthPaletteRle(runLength, &paletteRleData);
        runLength = 0;
      }
      paletteRleData.push_back(indexOfColor);
      previousIndexOfColor = indexOfColor;
    } else {
      runLength++;
      paletteRleData.back() |= 0x80;
    }
  }
  if (runLength > 0) {
    pushRunLengthPaletteRle(runLength, &paletteRleData);
  }

  m_oldSize = m_rgbData.size();
  m_rgbData.resize(m_oldSize + paletteRleData.size());
  memcpy(&m_rgbData[m_oldSize], &paletteRleData[0], paletteRleData.size());
}

void ZrleEncoder::pushRunLengthRle(int runLength)
{
  do {
    if (runLength > 255) {
      m_plainRleTile.push_back(255);
    } else {
      m_plainRleTile.push_back(runLength);
    }
    // Increase the size of palette RLE tile.
    m_paletteRleTileSize++;
    runLength -= 255;
  } while (runLength >= 0);
}

template <class PIXEL_T>
void ZrleEncoder::writePixelToPlainRleTile(const PIXEL_T px,
                                           PIXEL_T *previousPx)
{
  m_plainRleTile.resize(m_plainRleTile.size() + m_bytesPerPixel);
  memcpy(&m_plainRleTile[m_plainRleTile.size() - m_bytesPerPixel],
          &px + m_numberFirstByte,
          m_bytesPerPixel);
  *previousPx = px;
}

template <class PIXEL_T>
void ZrleEncoder::fillPalette(const Rect *tileRect,
                              const FrameBuffer *fb)
{
  // Clear the palette.
  m_pal.reset();
  m_pal.setMaxColors(MAX_NUMBER_OF_COLORS_IN_PALETTE);
  int tryInsertPx = 1;

  const PIXEL_T *buffer = (const PIXEL_T *)fb->getBuffer();
  PixelFormat pxFormat = fb->getPixelFormat();

  // Mask for cutting rubbish bits.
  PIXEL_T mask = pxFormat.redMax << pxFormat.redShift |
                 pxFormat.greenMax << pxFormat.greenShift |
                 pxFormat.blueMax << pxFormat.blueShift;

  // There is the first iteration of loop below.
  // Pixel for adding to plainRleTile
  PIXEL_T previousPx;
  PIXEL_T px = buffer[tileRect->top * m_fbWidth + tileRect->left];

  // Pixel for adding to palette
  PIXEL_T oldPixel = px;
  int palLength = 1;
  
  // Fill RLE tile vector.
  px &= mask;
  // Write type of subencoding.
  m_plainRleTile.push_back(128);

  // Calculate size of palette RLE tile.
  m_paletteRleTileSize = 1;
  writePixelToPlainRleTile<PIXEL_T>(px, &previousPx);

  // Increase the size of palette RLE tile.
  m_paletteRleTileSize++;

  int runLength = 0;
  for (int i = 1; i < tileRect->area(); ++i) {
    // FIXME: This variant may be not the most optimal.
    // One of the possible variant is double for loops.
    int x = tileRect->left + i % tileRect->getWidth();
    int y = tileRect->top + i / tileRect->getWidth();

    px = buffer[y * m_fbWidth + x];
    
    // Fill palette
    if (tryInsertPx && oldPixel != px) {
      tryInsertPx = m_pal.insert(oldPixel, palLength);
      oldPixel = px;
      palLength = 1;
    } else {
      palLength++;
    }
    
    // Fill RLE tile vector.
    px &= mask;
    if (px != previousPx) {
      pushRunLengthRle(runLength);
      runLength = 0;
      writePixelToPlainRleTile<PIXEL_T>(px, &previousPx);
    } else {
      runLength++;
    }
  }
  if (tryInsertPx) {
    m_pal.insert(oldPixel, palLength);
  }
  pushRunLengthRle(runLength);
}

template <class PIXEL_T>
void ZrleEncoder::copyPixels(const Rect *rect,
                             const FrameBuffer *fb,
                             UINT8 *dst)
{
  const int rectHeight = rect->getHeight();
  const int rectWidth = rect->getWidth();
  const PIXEL_T *src = static_cast<const PIXEL_T *>(fb->getBufferPtr(rect->left, rect->top));
  const int fbStride = fb->getDimension().width;
  const size_t bytesPerRow = rect->getWidth() * m_bytesPerPixel;

  for (int y = 0; y < rectHeight; y++) {
    memcpy(dst, src, bytesPerRow);
    src += fbStride;
    dst += bytesPerRow;
  }
}

void ZrleEncoder::copyCPixels(const Rect *rect,
                              const FrameBuffer *fb,
                              UINT8 *dst)
{
  const int rectHeight = rect->getHeight();
  const int rectWidth = rect->getWidth();
  const UINT8 *src = static_cast<const UINT8 *>(fb->getBufferPtr(rect->left, rect->top));
  const int fbStride = fb->getDimension().width;
  
  for (int y = 0; y < rectHeight; y++) {
    for (int x = 0; x < rectWidth; x++) {
      memcpy(dst, src + m_numberFirstByte, 3);
      src += 4;
      dst += 3;
    }
    src += ((fbStride - rectWidth) * 4);
  }
}
