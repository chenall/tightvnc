// Copyright (C) 2008,2009,2010,2011,2012 GlavSoft LLC.
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

#ifndef __RFB_JPEG_COMPRESSOR_H_INCLUDED__
#define __RFB_JPEG_COMPRESSOR_H_INCLUDED__

#include <stdio.h>

#include "util/CommonHeader.h"
#include "rfb/PixelFormat.h"

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

//
// An abstract interface for performing JPEG compression.
//

class JpegCompressor
{
public:
  virtual ~JpegCompressor() {}

  // Set JPEG quality level (0..100)
  virtual void setQuality(int level) = 0;
  virtual void resetQuality() = 0;

  // Actually compress a rectangle of a given pixel buffer referenced by buf.
  // The pixel format as specified by fmt must meet the following
  // requirements: bitsPerPixel must be either 32 or 16, and the bigEndian
  // flag must correspond to the native byte order. The stride value specifies
  // the number of bytes (not pixels!) occupied by one row of the buffer,
  // including padding if any.
  virtual void compress(const void *buf, const PixelFormat *fmt,
                        int w, int h, int stride) = 0;

  // Get the length of the output data, in bytes.
  virtual size_t getOutputLength() = 0;

  // Access the actual output of the compressor.
  virtual const char *getOutputData() = 0;
};

//
// StandardJpegCompressor is a C++ class for performing JPEG compression via
// the Independent JPEG Group's software (free JPEG library).
//
// See the documentation on JpegCompressor for information on the base methods
// overridden here.
//

class StandardJpegCompressor : public JpegCompressor
{
public:
  StandardJpegCompressor();
  virtual ~StandardJpegCompressor();

  virtual void setQuality(int level);
  virtual void resetQuality();

  virtual void compress(const void *buf, const PixelFormat *fmt,
                        int w, int h, int stride);

  virtual size_t getOutputLength();
  virtual const char *getOutputData();

public:
  // Our implementation of JPEG destination manager. These three
  // functions should never be called directly. They are made public
  // because they should be accessible from C-compatible functions
  // called by the JPEG library.
  void initDestination();
  bool emptyOutputBuffer();
  void termDestination();

protected:
  static const int ALLOC_CHUNK_SIZE;
  static const int DEFAULT_JPEG_QUALITY;

  int m_quality;
  int m_newQuality;

  unsigned char *m_outputBuffer;
  size_t m_numBytesAllocated;
  size_t m_numBytesReady;

  // Convert one row (scanline) from the specified pixel format to the format
  // supported by the IJG JPEG library (one byte per one color component).
  void convertRow(JSAMPLE *dst, const void *src,
                  const PixelFormat *fmt, int numPixels);

  // Convert one row (scanline) from the specified pixel format to the format
  // supported by the IJG JPEG library (one byte per one color component).
  // This is a faster version assuming that source pixels are 32-bit values
  // with actual color depth of 24 (redMax, greenMax and blueMax are all 255).
  void convertRow24(JSAMPLE *dst, const void *src,
                    const PixelFormat *fmt, int numPixels);

private:
  METHODDEF(StringStorage) getMessage(j_common_ptr cinfo);
  METHODDEF(void) errorExit(j_common_ptr cinfo);
  METHODDEF(void) outputMessage(j_common_ptr cinfo);

  typedef struct _TC_JPEG_COMPRESSOR {
    struct jpeg_compress_struct cinfo;
    struct jpeg_error_mgr jerr;
  } TC_JPEG_COMPRESSOR;

  TC_JPEG_COMPRESSOR m_jpeg;
};

#endif // __RFB_JPEG_COMPRESSOR_H_INCLUDED__
