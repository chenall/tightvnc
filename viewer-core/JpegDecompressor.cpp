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

#include "JpegDecompressor.h"

#include "util/AnsiStringStorage.h"
#include "util/Exception.h"

JpegDecompressor::JpegDecompressor()
{
  init();
}

JpegDecompressor::~JpegDecompressor()
{
  try {
    cleanup();
  } catch (...) {
  }
}

StringStorage JpegDecompressor::getMessage(j_common_ptr cinfo)
{
  char buffer[JMSG_LENGTH_MAX];
  // Create the message
  (*cinfo->err->format_message) (cinfo, buffer);

  AnsiStringStorage errorAnsi(buffer);
  StringStorage error;
  errorAnsi.toStringStorage(&error);
  return error;
}

void JpegDecompressor::errorExit(j_common_ptr cinfo)
{
  (*cinfo->err->output_message) (cinfo);
  StringStorage error = getMessage(cinfo);
  jpeg_destroy(cinfo);
  throw Exception(error.getString());
}

void JpegDecompressor::outputMessage(j_common_ptr cinfo)
{
  return;
}

void JpegDecompressor::decompress(vector<UINT8> &buffer,
                                  size_t jpegBufLen,
                                  vector<UINT8> &pixels,
                                  const Rect *dstRect)
{
  if (!dstRect->isValid())
    throw Exception(_T("invalid destination rectangle in jpeg-decompressor"));

  if (buffer.size() == 0 || buffer.size() < jpegBufLen)
    throw Exception(_T("incorrect size of buffer in jpeg-decompressor"));
  UINT8 *src_buf = &buffer.front();
  size_t src_buf_size = jpegBufLen;

  size_t width = dstRect->getWidth();
  size_t height = dstRect->getHeight();
  size_t pixelBufferCount =  width * height * BYTES_PER_PIXEL;
  if (pixels.size() == 0 || pixels.size() < pixelBufferCount)
    throw Exception(_T("incorrect size of pixels-buffer in jpeg-decompressor"));
  UINT8 *dst_buf = &pixels.front();

  try {
    /* Initialize data source and read the header. */
    jpeg_mem_src(&m_jpeg.cinfo, src_buf, static_cast<unsigned long>(src_buf_size));
    if (jpeg_read_header(&m_jpeg.cinfo, TRUE) != JPEG_HEADER_OK) {
      throw Exception(_T("possible, bad JPEG header"));
    }

    JDIMENSION jpegWidth = dstRect->getWidth();
    JDIMENSION jpegHeight = dstRect->getHeight();
    if (m_jpeg.cinfo.image_width != jpegWidth ||
        m_jpeg.cinfo.image_height != jpegHeight) {
      jpeg_abort_decompress(&m_jpeg.cinfo);
      throw Exception(_T("wrong image size"));
    }

    /* Configure and start decompression. */
    m_jpeg.cinfo.out_color_space = JCS_RGB;
    jpeg_start_decompress(&m_jpeg.cinfo);
    if (m_jpeg.cinfo.output_width != jpegWidth ||
        m_jpeg.cinfo.output_height != jpegHeight ) {
      jpeg_abort_decompress(&m_jpeg.cinfo);
      throw Exception(_T("something's wrong with the JPEG library"));
    }

    /* Consume decompressed data. */
    while (m_jpeg.cinfo.output_scanline < m_jpeg.cinfo.output_height) {
      size_t bufferIndex = m_jpeg.cinfo.output_scanline;
      size_t bufferOffset = bufferIndex * width * BYTES_PER_PIXEL;

      JSAMPROW row_ptr[1];
      row_ptr[0] = &dst_buf[bufferOffset];
      if (jpeg_read_scanlines(&m_jpeg.cinfo, row_ptr, 1) != 1) {
        jpeg_abort_decompress(&m_jpeg.cinfo);
        throw Exception(_T("error decompressing JPEG data"));
      }
    }
    /* Cleanup after the decompression. */
    jpeg_finish_decompress(&m_jpeg.cinfo);
  } catch (const Exception &ex) {
    cleanup();
    init();
    throw ex;
  }
}


void JpegDecompressor::init()
{
  jpeg_create_decompress(&m_jpeg.cinfo);
  
  m_jpeg.cinfo.err = jpeg_std_error(&m_jpeg.jerr);
  m_jpeg.cinfo.err->error_exit = errorExit;
  m_jpeg.cinfo.err->output_message = outputMessage;
}

void JpegDecompressor::cleanup()
{
  jpeg_destroy_decompress(&m_jpeg.cinfo);
}
