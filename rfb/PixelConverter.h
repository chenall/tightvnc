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

#ifndef __RFB_PIXEL_CONVERTER_H_INCLUDED__
#define __RFB_PIXEL_CONVERTER_H_INCLUDED__

#include "FrameBuffer.h"
#include "region/Point.h"

class PixelConverter
{
public:
  PixelConverter(void);
  virtual ~PixelConverter(void);

  // Convert pixels for the specified `rect' from `srcFb' to `dstFb'.
  // The pixel formats of `srcFb' and `dstFb' must be identical to the formats
  // set by the most recent setPixelFormats() call. The source and destination
  // framebuffers must be of the same size. The entire rectangle referenced by
  // `rect' must be within the frame buffer boundaries.
  virtual void convert(const Rect *rect, FrameBuffer *dstFb,
                       const FrameBuffer *srcFb) const;

  // Convert pixels for the specified `rect' from `srcFb' to the internal
  // PixelConverter's frame buffer and return a pointer to that frame buffer.
  // If the source and destination formats are the same, then no translation
  // will happen and srcFb will be returned.
  // The pixel format of `srcFb' must be identical to the source format set by
  // the most recent setPixelFormats() call. The entire rectangle referenced
  // by `rect' must be within the frame buffer boundaries.
  // This function will work efficiently only if the frame buffer size does
  // not vary from call to call. It checks if the frame buffer size has
  // changed since the previous call and reallocates the frame buffer if
  // necessary.
  virtual const FrameBuffer *convert(const Rect *rect,
                                     const FrameBuffer *srcFb);

  // FIXME: Review the argument order for each function of PixelConverter.
  // FIXME: Review the argument names for each function of PixelConverter.
  virtual void setPixelFormats(const PixelFormat *dstPf,
                               const PixelFormat *srcPf);

  // Return the number of bits per pixel from the source pixel format.
  virtual size_t getSrcBitsPerPixel() const;

  // Return the number of bits per pixel from the destination pixel format.
  virtual size_t getDstBitsPerPixel() const;

protected:
  void reset();

  void fillHexBitsTable(const PixelFormat *dstPf, const PixelFormat *srcPf);
  void fill32BitsTable(const PixelFormat *dstPf, const PixelFormat *srcPf);
  UINT32 rotateUint32(UINT32 value) const;

  enum ConvertMode
  {
    NO_CONVERT,
    CONVERT_FROM_16,
    CONVERT_FROM_32
  };

  ConvertMode m_convertMode;
  std::vector<UINT32> m_hexBitsTable;
  std::vector<UINT32> m_redTable;
  std::vector<UINT32> m_grnTable;
  std::vector<UINT32> m_bluTable;

  PixelFormat m_srcFormat;
  PixelFormat m_dstFormat;

  // An internally maintained frame buffer used by the two-argument version of
  // the convert() function.
  FrameBuffer *m_dstFrameBuffer;
};

#endif // __RFB_PIXEL_CONVERTER_H_INCLUDED__
