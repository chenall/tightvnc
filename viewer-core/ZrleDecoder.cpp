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

#include "ZrleDecoder.h"

#include "io-lib/ByteArrayInputStream.h"

#include <vector>
#include <algorithm>

ZrleDecoder::ZrleDecoder(LogWriter *logWriter)
: DecoderOfRectangle(logWriter)
{
  m_encoding = EncodingDefs::ZRLE;
}

ZrleDecoder::~ZrleDecoder()
{
}

void ZrleDecoder::decode(RfbInputGate *input,
                         FrameBuffer *frameBuffer,
                         const Rect *dstRect)
{
  size_t maxUnpackedSize = getMaxSizeOfRectangle(dstRect);
  readAndInflate(input, maxUnpackedSize);

  size_t unpackedDataSize = m_inflater.getOutputSize();
  if (unpackedDataSize == 0) {
    m_logWriter->debug(_T("Empty unpacked data (zrle-decoder)"));
    if (dstRect->area() != 0) {
      m_logWriter->detail(_T("Corrupted data in zrle-decoder, rectangle is undefined."));
      m_logWriter->detail(_T("Possible, data is corrupted or error in server"));
    }
    return;
  }

  vector<unsigned char> unpackedData;
  unpackedData.resize(unpackedDataSize);
  unpackedData.assign(m_inflater.getOutput(), m_inflater.getOutput() + unpackedDataSize);
  ByteArrayInputStream unpackedByteArrayStream(reinterpret_cast<char *>(&unpackedData.front()),
                                               unpackedData.size());
  DataInputStream unpackedDataStream(&unpackedByteArrayStream);

  m_numberFirstByte = 0;
  PixelFormat pxFormat = frameBuffer->getPixelFormat();

  if (pxFormat.bitsPerPixel == 8) {
    m_bytesPerPixel = 1;
  } else if (pxFormat.bitsPerPixel == 16) {
    m_bytesPerPixel = 2;
  } else if (pxFormat.bitsPerPixel == 32) {
    UINT32 colorMaxValue =  pxFormat.blueMax  << pxFormat.blueShift  |
                            pxFormat.greenMax << pxFormat.greenShift |
                            pxFormat.redMax   << pxFormat.redShift;
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

  for (int y = dstRect->top; y < dstRect->bottom; y += TILE_SIZE) {
    for (int x = dstRect->left; x < dstRect->right; x += TILE_SIZE) {
      Rect tileRect(x, y, 
                    std::min(x + TILE_SIZE, dstRect->right),
                    std::min(y + TILE_SIZE, dstRect->bottom));

      if (!frameBuffer->getDimension().getRect().intersection(&tileRect).isEqualTo(&tileRect)) {
        throw Exception(_T("Error in protocol: incorrect size of tile (zrle-decoder)"));
      }
      size_t tileLength = tileRect.area();
      size_t tileBytesLength = tileLength * m_bytesPerPixel;
      vector<char> pixels;
      pixels.resize(tileBytesLength);

      int type = readType(&unpackedDataStream);

      if (type == 0) {
        // raw pixel data
        readRawTile(&unpackedDataStream, pixels, &tileRect);
      } else if (type == 1) {
        // a solid tile consisting of a single colour
        readSolidTile(&unpackedDataStream, pixels, &tileRect);
      } else if (type >= 2 && type <= 16) {
        // packed palette
        readPackedPaletteTile(&unpackedDataStream, pixels, &tileRect, type);
      } else if (type >= 17 && type <= 127) {
        // unused (no advantage over palette RLE)
      } if (type == 128) {
        // plain rle
        readPlainRleTile(&unpackedDataStream, pixels, &tileRect);
      } if (type == 129) {
        // invalid type
        StringStorage error;
        error.format(_T("Error: subencoding %d of Zrle encoding is unused"), type);
        throw Exception(error.getString());
      } if (type >= 130 && type <= 255) {
        // palette rle
        readPaletteRleTile(&unpackedDataStream, pixels, &tileRect, type);
      }

      drawTile(frameBuffer, &tileRect, &pixels);
    } // tile(x, y)
  } // tile(..., y)
}

void ZrleDecoder::readAndInflate(RfbInputGate *input, size_t maximalUnpackedSize)
{
  UINT32 length = input->readUInt32();
  std::vector<char> zlibData;
  zlibData.resize(length);
  if (length == 0) {
    zlibData.resize(1);
  }
  input->readFully(&zlibData.front(), length);

  m_inflater.setInput(&zlibData.front(), length);
  m_inflater.setUnpackedSize(maximalUnpackedSize);
  m_inflater.inflate();
}

size_t ZrleDecoder::getMaxSizeOfRectangle(const Rect *dstRect)
{
  size_t widthCount = (dstRect->getWidth() + TILE_SIZE - 1) / TILE_SIZE;
  size_t heightCount = (dstRect->getHeight() + TILE_SIZE - 1) / TILE_SIZE;
  size_t tileCount = widthCount * heightCount;
  return TILE_LENGTH_SIZE + MAXIMAL_TILE_SIZE * tileCount;
}

int ZrleDecoder::readType(DataInputStream *input)
{
  int type = input->readUInt8();
  return type;
}

size_t ZrleDecoder::readRunLength(DataInputStream *input)
{
  size_t runLength = 0;
  UINT8 delta;
  do {
    delta = input->readUInt8();
    runLength += delta;
  } while (delta == 255); // if value == 255 then continue reading run-length
  return runLength + 1; // the length is one more than the sum
}

void ZrleDecoder::readPalette(DataInputStream *input,
                              const int paletteSize,
                              Palette *palette)
{
  palette->resize(paletteSize);

  for (int i = 0; i < paletteSize; i++) {
    input->readFully(&(*palette)[i] + m_numberFirstByte, m_bytesPerPixel);
  }
}

void ZrleDecoder::readRawTile(DataInputStream *input,
                              vector<char> &pixels,
                              const Rect *tileRect)
{
  size_t tileBytesLength = tileRect->area() * m_bytesPerPixel;
  input->readFully(&pixels.front(), tileBytesLength);
}

void ZrleDecoder::readSolidTile(DataInputStream *input,
                                vector<char> &pixels,
                                const Rect *tileRect)
{
  size_t tileLength = tileRect->area();
  char solid[4] = {0, 0, 0, 0};

  input->readFully(solid + m_numberFirstByte, m_bytesPerPixel);
  
  char *pixelsPtr = &pixels.front();
  // TODO: Can we optimize this?
  for (size_t i = 0; i < tileLength; i++) {
    memcpy(pixelsPtr + m_numberFirstByte, solid, m_bytesPerPixel); 
    pixelsPtr += m_bytesPerPixel;
  }
}

void ZrleDecoder::readPackedPaletteTile(DataInputStream *input,
                                        vector<char> &pixels,
                                        const Rect *tileRect,
                                        const int type)
{
  int width = tileRect->getWidth();
  int height = tileRect->getHeight();

  // type and palette size is equal
  int paletteSize = type;
  Palette palette;
  readPalette(input, paletteSize, &palette);

  int m = 0;
  unsigned char mask = 0;
  unsigned char deltaOffset = 0;
  if (paletteSize == 2) {
    m = (width + 7) / 8;
    mask = 0x01;
    deltaOffset = 1;
  } else if (paletteSize == 3 || paletteSize == 4) {
    m = (width + 3) / 4;
    mask = 0x03;
    deltaOffset = 2;
  } else if (paletteSize >= 5 && paletteSize <= 16) {
    m = (width + 1) / 2;
    mask = 0x0F;
    deltaOffset = 4;
  }

  for (int y = 0; y < height; y++) {
    // bit lenght of UINT8
    unsigned char offset = 8;
    int index = 0;
    // FIXME: optimization. Read by line.
    int entryByIndex = input->readUInt8();

    for (int x = 0; x < width; x++) {
      offset -= deltaOffset;
      int color = (entryByIndex >> offset) & mask;
      if (offset == 0) {
        offset = 8;
        // Don't read next entry, if it's last pixel in tile.
        if (x != width - 1) {
          entryByIndex = input->readUInt8();
        }
      }

      size_t count = y * width + x;
      memcpy(&pixels[count * m_bytesPerPixel], &palette[color], m_bytesPerPixel);
    }
  }
}

void ZrleDecoder::readPlainRleTile(DataInputStream *input,
                                   vector<char> &pixels,
                                   const Rect *tileRect)
{
  size_t tileLength = tileRect->area();
  for (size_t indexPixel = 0; indexPixel < tileLength * m_bytesPerPixel;) {
    char color[4] = {0, 0, 0, 0};
    input->readFully(color + m_numberFirstByte, m_bytesPerPixel);

    size_t runLength = readRunLength(input);
    // TODO: refactor this
    for(size_t i = 0; i < runLength; i++) {
      // FIXME: add check this condition in all similar areas.
      if (indexPixel + m_bytesPerPixel <= pixels.size()) {
        memcpy(&pixels[indexPixel], color, m_bytesPerPixel);
      } else {
        throw Exception(_T("Corrupt protocol in Zrle-decoder (plain rle tile)."));
      }
      indexPixel += m_bytesPerPixel; 
    }
  }
}

void ZrleDecoder::readPaletteRleTile(DataInputStream *input,
                                     vector<char> &pixels,
                                     const Rect *tileRect,
                                     const int type)
{
  size_t tileLength = tileRect->area();

  int paletteSize = type - 128;
  Palette palette;
  readPalette(input, paletteSize, &palette);

  for (size_t indexPixel = 0; indexPixel < tileLength;) {
    UINT8 color = input->readUInt8();

    size_t runLength = 1;
    if (color >= 128) {
      color -= 128;
      runLength = readRunLength(input);
    }
    
    char * pixelsPtr = &pixels[indexPixel * m_bytesPerPixel];
    for(size_t i = 0; i < runLength; i++) {
      memcpy(pixelsPtr, &palette[color], m_bytesPerPixel);
      pixelsPtr += m_bytesPerPixel;
    }
    indexPixel += runLength;
  }
}

void ZrleDecoder::drawTile(FrameBuffer *fb,
                           const Rect *tileRect,
                           const vector<char> *pixels)
{
  int width = tileRect->getWidth();
  size_t fbBytesPerPixel = m_bytesPerPixel;

  if (fbBytesPerPixel == 3) {
    fbBytesPerPixel = 4;
  }

  int tileLength = tileRect->area();

  int x = tileRect->left;
  int y = tileRect->top;
   
  const char *pixelsPtr = &pixels->front();
  char *bufferPtr = 0;
  for (int i = 0; i < tileLength; i++) {
    bufferPtr = (char*)fb->getBufferPtr(x + i % width, y + i / width);
    memset(bufferPtr, 0, fbBytesPerPixel);
    memcpy(bufferPtr, pixelsPtr, m_bytesPerPixel);
    pixelsPtr+=m_bytesPerPixel;
  }
}
