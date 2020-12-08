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

#ifndef __RFB_ZRLE_ENCODER_H_INCLUDED__
#define __RFB_ZRLE_ENCODER_H_INCLUDED__

#include "Encoder.h"
#include "TightPalette.h"
#include "util/Deflater.h"

class ZrleEncoder : public Encoder
{
public:
  ZrleEncoder(PixelConverter *conv, DataOutputStream *output);
  virtual ~ZrleEncoder();

  // Follow methods were inherited from the Encoder.
  virtual int getCode() const;

  virtual void splitRectangle(const Rect *rect,
                              std::vector<Rect> *rectList,
                              const FrameBuffer *serverFb,
                              const EncodeOptions *options);
  
  virtual void sendRectangle(const Rect *rect,
                             const FrameBuffer *serverFb,
                             const EncodeOptions *options) throw(IOException);

private:
  // Determine the class of rectangle and call necessary function for this type.
  template <class PIXEL_T>
    void sendRect(const Rect *rect,
                  const FrameBuffer *serverFb,
                  const FrameBuffer *clientFb,
                  const EncodeOptions *options) throw(IOException);

  // Send raw tile.
  template <class PIXEL_T>
    void writeRawTile(const Rect *tileRect,
                      const FrameBuffer *fb) throw(IOException);

  // Send a solid-color tile.
    void writeSolidTile() throw(IOException);
  
  // Send packed palette tile.
  template <class PIXEL_T>
    void writePackedPaletteTile(const Rect *tileRect,
                                const FrameBuffer *fb) throw(IOException);

  // Send palette RLE tile.
  template <class PIXEL_T>
    void writePaletteRleTile(const Rect *tileRect,
                             const FrameBuffer *fb) throw(IOException);

  // Write data from runLength (used in plain Rle encoding).
  void pushRunLengthRle(int runLength);

  // Write data from runLength (used in palette Rle encoding).
  void pushRunLengthPaletteRle(int runLength,
                                 std::vector<UINT8> *paletteRleData);

  // Write pixel to the plainRleTile.
  template <class PIXEL_T>
    void writePixelToPlainRleTile(const PIXEL_T px,
                                  PIXEL_T *previousPx);

  // Fill palette (m_pal), create m_plainRleTile vector and calculate size of data in palette RLE tile.
  template <class PIXEL_T>
    void fillPalette(const Rect *tileRect,
                     const FrameBuffer *fb);

  // Copy ordinary PIXELs.
  template <class PIXEL_T>
    void copyPixels(const Rect *rect,
                    const FrameBuffer *fb,
                    UINT8 *dst);
  
  // Copy CPIXELs.
  void copyCPixels(const Rect *rect,
                   const FrameBuffer *fb,
                   UINT8 *dst);

  // Vector for storing all tiles for the future zlib compression.
  std::vector<UINT8> m_rgbData;
  // Size of m_rgbData before writing information in it.
  size_t m_oldSize;

  // Size of stride.
  int m_fbWidth;

  // Size of packed pixels in palette.
  int m_mSize;
  
  // The only pixel format type for whole rectangle.
  PixelFormat m_pxFormat;

  // Used for determing: is it CPIXEL or PIXEL.
  size_t m_bytesPerPixel;
  size_t m_numberFirstByte;

  // Zlib object and settings for the it.
  Deflater m_deflater;
  int m_idxZlibLevel;
  int m_monoZlibLevel;
  int m_rawZlibLevel;
  
  // Color palette.
  TightPalette m_pal;

  // Variables for storing size of all tiles.
  size_t m_rawTileSize;
  size_t m_paletteTileSize;
  size_t m_paletteRleTileSize;

  // vector for storing plain RLE tile data
  std::vector<UINT8> m_plainRleTile;

private:
  // Tile size in ZRLE encoding by default.
  static const int TILE_SIZE = 64;

  // Default values for zlib settings.
  static const int ZLIB_IDX_LEVEL_DEFAULT = 7;
  static const int ZLIB_MONO_LEVEL_DEFAULT = 7;
  static const int ZLIB_RAW_LEVEL_DEFAULT = 6;

  // Description of tile max size is located in ZrleDecoder.h.
  // The max possible size of tile is 20481, so 20482 is not possible.
  static const int THIS_TYPE_OF_TILE_IS_NOT_POSSIBLE = 20482;

  // Max possible colors in palette (127 is max for RLE palette type encoding).
  static const UINT8 MAX_NUMBER_OF_COLORS_IN_PALETTE = 127;
};

#endif // __RFB_ZRLE_ENCODER_H_INCLUDED__
