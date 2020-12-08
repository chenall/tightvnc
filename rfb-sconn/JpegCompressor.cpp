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

#include "JpegCompressor.h"

#include "util/AnsiStringStorage.h"
#include "util/Exception.h"

const int StandardJpegCompressor::ALLOC_CHUNK_SIZE = 65536;
const int StandardJpegCompressor::DEFAULT_JPEG_QUALITY = 75;

//
// Extend jpeg_destination_mgr struct with a pointer to our object.
//

typedef struct {
  struct jpeg_destination_mgr pub;
  StandardJpegCompressor *_this;
} my_destination_mgr;

//
// C-compatible interface to our destination manager. It just obtains
// a pointer to the right object and calls a corresponding C++ member
// function on that object.
//

static void
init_destination(j_compress_ptr cinfo)
{
  my_destination_mgr *dest_ptr = (my_destination_mgr *)cinfo->dest;
  dest_ptr->_this->initDestination();
}

static boolean
empty_output_buffer (j_compress_ptr cinfo)
{
  my_destination_mgr *dest_ptr = (my_destination_mgr *)cinfo->dest;
  return (boolean)dest_ptr->_this->emptyOutputBuffer();
}

static void
term_destination (j_compress_ptr cinfo)
{
  my_destination_mgr *dest_ptr = (my_destination_mgr *)cinfo->dest;
  dest_ptr->_this->termDestination();
}

//
// Constructor and destructor.
//

StandardJpegCompressor::StandardJpegCompressor()
  : m_quality(-1), // make sure (m_quality != n_newQuality)
    m_newQuality(DEFAULT_JPEG_QUALITY),
    m_outputBuffer(0),
    m_numBytesAllocated(0),
    m_numBytesReady(0)
{
  // Initialize JPEG compression structure.
  jpeg_create_compress(&m_jpeg.cinfo);

  m_jpeg.cinfo.err = jpeg_std_error(&m_jpeg.jerr);
  m_jpeg.cinfo.err->error_exit = errorExit;
  m_jpeg.cinfo.err->output_message = outputMessage;

  // Set up a destination manager.
  my_destination_mgr *dest = new my_destination_mgr;
  dest->pub.init_destination = init_destination;
  dest->pub.empty_output_buffer = empty_output_buffer;
  dest->pub.term_destination = term_destination;
  dest->_this = this;
  m_jpeg.cinfo.dest = (jpeg_destination_mgr *)dest;

  // Set up compression parameters. Do not set quality level here,
  // it will be set right before the compression.
  m_jpeg.cinfo.input_components = 3;
  m_jpeg.cinfo.in_color_space = JCS_RGB;
  jpeg_set_defaults(&m_jpeg.cinfo);

  // We prefer speed over quality.
  m_jpeg.cinfo.dct_method = JDCT_FASTEST;
}

StandardJpegCompressor::~StandardJpegCompressor()
{
  // Free compressed data buffer.
  if (m_outputBuffer)
    free(m_outputBuffer);

  // Clean up the destination manager.
  delete m_jpeg.cinfo.dest;
  m_jpeg.cinfo.dest = NULL;

  // Release the JPEG compression structure.
  jpeg_destroy_compress(&m_jpeg.cinfo);
}

StringStorage StandardJpegCompressor::getMessage(j_common_ptr cinfo)
{
  char buffer[JMSG_LENGTH_MAX];
  // Create the message
  (*cinfo->err->format_message) (cinfo, buffer);

  AnsiStringStorage errorAnsi(buffer);
  StringStorage error;
  errorAnsi.toStringStorage(&error);
  return error;
}

void StandardJpegCompressor::errorExit(j_common_ptr cinfo)
{
  (*cinfo->err->output_message) (cinfo);
  StringStorage error = getMessage(cinfo);
  jpeg_destroy(cinfo);
  throw Exception(error.getString());
}

void StandardJpegCompressor::outputMessage(j_common_ptr cinfo)
{
  return;
}

//
// Our implementation of destination manager.
//

void
StandardJpegCompressor::initDestination()
{
  if (!m_outputBuffer) {
    size_t newSize = ALLOC_CHUNK_SIZE;
    m_outputBuffer = (unsigned char *)malloc(newSize);
    m_numBytesAllocated = newSize;
  }

  m_numBytesReady = 0;
  m_jpeg.cinfo.dest->next_output_byte = m_outputBuffer;
  m_jpeg.cinfo.dest->free_in_buffer =  m_numBytesAllocated;
}

bool
StandardJpegCompressor::emptyOutputBuffer()
{
  size_t oldSize = m_numBytesAllocated;
  size_t newSize = oldSize + ALLOC_CHUNK_SIZE;

  m_outputBuffer = (unsigned char *)realloc(m_outputBuffer, newSize);
  m_numBytesAllocated = newSize;

  m_jpeg.cinfo.dest->next_output_byte = &m_outputBuffer[oldSize];
  m_jpeg.cinfo.dest->free_in_buffer = newSize - oldSize;

  return true;
}

void
StandardJpegCompressor::termDestination()
{
  m_numBytesReady = m_numBytesAllocated - m_jpeg.cinfo.dest->free_in_buffer;
}

//
// Set JPEG quality level (0..100)
//

void
StandardJpegCompressor::setQuality(int level)
{
  if (level < 0) {
    level = 0;
  } else if (level > 100) {
    level = 100;
  }
  m_newQuality = level;
}

//
// Reset JPEG quality level to the default value.
//

void
StandardJpegCompressor::resetQuality()
{
  m_newQuality = DEFAULT_JPEG_QUALITY;
}

void
StandardJpegCompressor::compress(const void *buf,
                                 const PixelFormat *fmt,
                                 int w, int h, int stride)
{
  bool useQuickConversion =
    (fmt->bitsPerPixel == 32 && fmt->colorDepth == 24 &&
     fmt->redMax == 255 && fmt->greenMax == 255 && fmt->blueMax == 255);

  m_jpeg.cinfo.image_width = w;
  m_jpeg.cinfo.image_height = h;

  if (m_newQuality != m_quality) {
    jpeg_set_quality(&m_jpeg.cinfo, m_newQuality, true);
    m_quality = m_newQuality;
  }

  jpeg_start_compress(&m_jpeg.cinfo, TRUE);

  const char *src = (const char *)buf;

  // We'll pass up to 8 rows to jpeg_write_scanlines().
  JSAMPLE *rgb = new JSAMPLE[w * 3 * 8];
  JSAMPROW rowPointer[8];
  for (int i = 0; i < 8; i++)
    rowPointer[i] = &rgb[w * 3 * i];

  // Feed the pixels to the JPEG library.
  while (m_jpeg.cinfo.next_scanline < m_jpeg.cinfo.image_height) {
    int maxRows = m_jpeg.cinfo.image_height - m_jpeg.cinfo.next_scanline;
    if (maxRows > 8) {
      maxRows = 8;
    }
    for (int dy = 0; dy < maxRows; dy++) {
      if (useQuickConversion) {
        convertRow24(rowPointer[dy], src, fmt, w);
      } else {
        convertRow(rowPointer[dy], src, fmt, w);
      }
      src += stride;
    }
    jpeg_write_scanlines(&m_jpeg.cinfo, rowPointer, maxRows);
  }

  delete[] rgb;

  jpeg_finish_compress(&m_jpeg.cinfo);
}

size_t StandardJpegCompressor::getOutputLength()
{
  return m_numBytesReady;
}

const char *StandardJpegCompressor::getOutputData()
{
  return (const char *)m_outputBuffer;
}

void
StandardJpegCompressor::convertRow24(JSAMPLE *dst, const void *src,
                                     const PixelFormat *fmt, int numPixels)
{
  const UINT32 *srcPixels = (const UINT32 *)src;
  while (numPixels--) {
    UINT32 pixel = *srcPixels++;
    *dst++ = (JSAMPLE)(pixel >> fmt->redShift);
    *dst++ = (JSAMPLE)(pixel >> fmt->greenShift);
    *dst++ = (JSAMPLE)(pixel >> fmt->blueShift);
  }
}

void
StandardJpegCompressor::convertRow(JSAMPLE *dst, const void *src,
                                   const PixelFormat *fmt, int numPixels)
{
  if (fmt->bitsPerPixel == 32) {
    const UINT32 *srcPixels = (const UINT32 *)src;
    for (int x = 0; x < numPixels; x++) {
      UINT32 pixel = *srcPixels++;
      *dst++ = (JSAMPLE)((pixel >> fmt->redShift & fmt->redMax) * 255 / fmt->redMax);
      *dst++ = (JSAMPLE)((pixel >> fmt->greenShift & fmt->greenMax) * 255 / fmt->greenMax);
      *dst++ = (JSAMPLE)((pixel >> fmt->blueShift & fmt->blueMax) * 255 / fmt->blueMax);
    }
  } else { // assuming (fmt->bitsPerPixel == 16)
    const UINT16 *srcPixels = (const UINT16 *)src;
    for (int x = 0; x < numPixels; x++) {
      UINT16 pixel = *srcPixels++;
      *dst++ = (JSAMPLE)((pixel >> fmt->redShift & fmt->redMax) * 255 / fmt->redMax);
      *dst++ = (JSAMPLE)((pixel >> fmt->greenShift & fmt->greenMax) * 255 / fmt->greenMax);
      *dst++ = (JSAMPLE)((pixel >> fmt->blueShift & fmt->blueMax) * 255 / fmt->blueMax);
    }
  }
}
