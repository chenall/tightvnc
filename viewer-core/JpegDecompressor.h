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

#ifndef _JPEG_DECOMPRESSOR_H_
#define _JPEG_DECOMPRESSOR_H_

#include <cstdio>

#include "region/Rect.h"

// More help of jpeg-lib in /usr/share/doc/jpeg-8c-r1/example.c.bz2

// For Windows platforms only.
// For using libjpeg for encoding go to Property Pages of tvnserver -> Linker -> Input -> Additional Dependencies
// and delete "$(SolutionDir)libjpeg-turbo\jpeg-static.lib" and "$(SolutionDir)libjpeg-turbo\jpeg-static-64.lib"
// (without quotes) from the end of all configurations.
// For Visual Studio 2008 add libjpeg project to the dependencies of tvnserver.
// Also you need to delete LIBJPEG_TURBO directive from the CommonHeader.h file.

#ifdef LIBJPEG_TURBO
#include "libjpeg-turbo/jpeglib.h"
#else
#include "libjpeg/jpeglib.h"
#endif

class JpegDecompressor
{
public:
  static const size_t BYTES_PER_PIXEL = 3;
public:
  JpegDecompressor();
  virtual ~JpegDecompressor();

  /*
   * Decompress JPEG data from the given buffer. Image size must be known in
   * advance, and dst_buf should have place for (w * h * 3) bytes. The output
   * format is an array of bytes where each pixel is represented by three bytes
   * for red, green and blue components, in that order.
   */
  void decompress(vector<UINT8> &buffer,
                  size_t jpegBufLen,
                  vector<UINT8> &pixels,
                  const Rect *dstRect);

private:
  /*
   * Initialize JPEG decoder. This function initializes the TD_JPEG_DECOMPRESSOR
   * structure. 
   */
  void init();

  /* Release the JPEG decompression structure. */
  /*
   * Free all dynamically allocated data associated with this TD_JPEG_DECOMPRESSOR
   * structure. This function should be called when the decoder is not needed
   * any more.
   */
  void cleanup();

private:
  METHODDEF(StringStorage) getMessage(j_common_ptr cinfo);
  METHODDEF(void) errorExit(j_common_ptr cinfo);
  METHODDEF(void) outputMessage(j_common_ptr cinfo);

  typedef struct _TD_JPEG_DECOMPRESSOR {
    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;
  } TD_JPEG_DECOMPRESSOR;

  TD_JPEG_DECOMPRESSOR m_jpeg;
};

#endif
