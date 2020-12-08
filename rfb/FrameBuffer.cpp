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

#include "FrameBuffer.h"
#include <string.h>

FrameBuffer::FrameBuffer(void)
: m_buffer(0)
{
  memset(&m_pixelFormat, 0, sizeof(m_pixelFormat));
}

FrameBuffer::~FrameBuffer(void)
{
  if (m_buffer != 0) {
    delete []m_buffer;
  }
}

bool FrameBuffer::assignProperties(const FrameBuffer *srcFrameBuffer)
{
  setProperties(&srcFrameBuffer->getDimension(),
                &srcFrameBuffer->getPixelFormat());
  return resizeBuffer();
}

bool FrameBuffer::clone(const FrameBuffer *srcFrameBuffer)
{
  if (!assignProperties(srcFrameBuffer)) {
    return false;
  }

  Rect fbRect = &m_dimension.getRect();
  copyFrom(&fbRect, srcFrameBuffer, fbRect.left, fbRect.top);

  return true;
}

void FrameBuffer::setColor(UINT8 red, UINT8 green, UINT8 blue)
{
  size_t sizeInPixels = m_dimension.area();
  int pixelSize = m_pixelFormat.bitsPerPixel / 8;
  UINT32 redPix = (red * m_pixelFormat.redMax / 255) <<
                  m_pixelFormat.redShift;
  UINT32 greenPix = (green * m_pixelFormat.greenMax / 255) <<
                    m_pixelFormat.greenShift;
  UINT32 bluePix = (blue * m_pixelFormat.blueMax / 255) <<
                   m_pixelFormat.blueShift;
  UINT32 color = redPix | greenPix | bluePix;

  UINT8 *endPixPtr = (UINT8 *)m_buffer + getBufferSize();
  UINT8 *pixPtr = (UINT8 *)m_buffer;
  for (; pixPtr <= endPixPtr - pixelSize; pixPtr += pixelSize) {
    memcpy(pixPtr, &color, pixelSize);
  }
}

void FrameBuffer::fillRect(const Rect *dstRect, UINT32 color)
{
  Rect clipRect = m_dimension.getRect().intersection(dstRect);

  int pixelSize = getBytesPerPixel();
  size_t sizeLineFb = getBytesPerRow();
  size_t sizeLineRect = clipRect.getWidth() * pixelSize;

  // it's pointer to first line of rect
  UINT8 *srcLinePtr = reinterpret_cast<UINT8 *>(getBufferPtr(clipRect.left, clipRect.top));

  // it's pointer to pixel in first line
  UINT8 *pixPtr = srcLinePtr;
  for (int x = clipRect.left; x < clipRect.right; x++, pixPtr += pixelSize)
    memcpy(pixPtr, &color, pixelSize);

  // it's pointer to next line of rect
  UINT8 *dstLinePtr = srcLinePtr + sizeLineFb;
  for (int y = clipRect.top + 1; y < clipRect.bottom; y++, dstLinePtr += sizeLineFb)
    memcpy(dstLinePtr, srcLinePtr, sizeLineRect);
}

bool FrameBuffer::isEqualTo(const FrameBuffer *frameBuffer)
{
  return m_dimension.cmpDim(&frameBuffer->getDimension()) &&
         m_pixelFormat.isEqualTo(&frameBuffer->getPixelFormat());
}

void FrameBuffer::clipRect(const Rect *dstRect, const FrameBuffer *srcFrameBuffer,
                           const int srcX, const int srcY,
                           Rect *dstClippedRect, Rect *srcClippedRect)
{
  Rect srcBufferRect = srcFrameBuffer->getDimension().getRect();
  clipRect(dstRect, &srcBufferRect, srcX, srcY, dstClippedRect, srcClippedRect);
}

void FrameBuffer::clipRect(const Rect *dstRect, const Rect *srcBufferRect,
                           const int srcX, const int srcY,
                           Rect *dstClippedRect, Rect *srcClippedRect)
{
  Rect dstBufferRect = m_dimension.getRect();

  // Building srcRect
  Rect srcRect(srcX, srcY, srcX + dstRect->getWidth(), srcY + dstRect->getHeight());

  // Finding common area between the dstRect, srcRect and the FrameBuffers
  Rect dstRectFB = dstBufferRect.intersection(dstRect);
  Rect srcRectFB = srcBufferRect->intersection(&srcRect);

  // Finding common area between the dstRectFB and the srcRectFB
  Rect dstCommonArea(&dstRectFB);
  Rect srcCommonArea(&srcRectFB);
  // Move to common place (left = 0, top = 0)
  dstCommonArea.move(-dstRect->left, -dstRect->top);
  srcCommonArea.move(-srcRect.left, -srcRect.top);

  Rect commonRect(&dstCommonArea.intersection(&srcCommonArea));

  // Moving commonRect to destination coordinates and source
  dstClippedRect->setRect(&commonRect);
  dstClippedRect->move(dstRect->left, dstRect->top);

  srcClippedRect->setRect(&commonRect);
  srcClippedRect->move(srcRect.left, srcRect.top);
}

bool FrameBuffer::overlay(const Rect *dstRect,
                          const FrameBuffer *srcFrameBuffer,
                          int srcX, int srcY,
                          const char *andMask)
{
  if (!m_pixelFormat.isEqualTo(&srcFrameBuffer->getPixelFormat())) {
    return false;
  }
  if (m_pixelFormat.bitsPerPixel == 32) {
    return overlayT<UINT32>(dstRect, srcFrameBuffer, srcX, srcY, andMask);
  } else if (m_pixelFormat.bitsPerPixel == 16) {
    return overlayT<UINT16>(dstRect, srcFrameBuffer, srcX, srcY, andMask);
  } else if (m_pixelFormat.bitsPerPixel == 8) {
    return overlayT<UINT8>(dstRect, srcFrameBuffer, srcX, srcY, andMask);
  } else {
    _ASSERT(false);
  }
  return false;
}

template<class PIXEL_T> bool FrameBuffer::overlayT(const Rect *dstRect,
                                                   const FrameBuffer *srcFrameBuffer,
                                                   int srcX, int srcY,
                                                   const char *andMask)
{
  Rect srcClippedRect, dstClippedRect;

  clipRect(dstRect, srcFrameBuffer, srcX, srcY, &dstClippedRect, &srcClippedRect);
  if (dstClippedRect.area() <= 0 || srcClippedRect.area() <= 0) {
    return true;
  }

  PIXEL_T *dstPixels = (PIXEL_T *)getBuffer();
  PIXEL_T *srcPixels = (PIXEL_T *)srcFrameBuffer->getBuffer();
  int srcWidth = srcFrameBuffer->getDimension().width;
  int dstWidth = getDimension().width;
  size_t bytesPerRow = (srcWidth + 7) / 8;
  for (int iRow = srcClippedRect.top; iRow < srcClippedRect.bottom; iRow++) {
    for (int iCol = srcClippedRect.left; iCol < srcClippedRect.right; iCol++) {
      unsigned char curByte = andMask[iRow * bytesPerRow + iCol / 8];
      bool andBit = (curByte & 128 >> iCol % 8) != 0;
      if (andBit) {
        int iDstRow = dstClippedRect.top + iRow - srcY - srcClippedRect.top;
        int iDstCol = dstClippedRect.left + iCol - srcX - srcClippedRect.left;
        dstPixels[iDstRow * dstWidth + iDstCol] = srcPixels[iRow * srcWidth + iCol];
      }
    }
  }
  return true;
}

bool FrameBuffer::copyFrom(const Rect *dstRect, const FrameBuffer *srcFrameBuffer,
                           int srcX, int srcY)
{
  if (!m_pixelFormat.isEqualTo(&srcFrameBuffer->getPixelFormat())) {
    return false;
  }

  Rect srcClippedRect, dstClippedRect;

  clipRect(dstRect, srcFrameBuffer, srcX, srcY, &dstClippedRect, &srcClippedRect);
  if (dstClippedRect.area() <= 0 || srcClippedRect.area() <= 0) {
    return true;
  }

  // Shortcuts
  int pixelSize = m_pixelFormat.bitsPerPixel / 8;
  int dstStrideBytes = m_dimension.width * pixelSize;
  int srcStrideBytes = srcFrameBuffer->getDimension().width * pixelSize;

  int resultHeight = dstClippedRect.getHeight();
  int resultWidthBytes = dstClippedRect.getWidth() * pixelSize;

  UINT8 *pdst = (UINT8 *)m_buffer
                + dstClippedRect.top * dstStrideBytes
                + pixelSize * dstClippedRect.left;

  UINT8 *psrc = (UINT8 *)srcFrameBuffer->getBuffer()
                + srcClippedRect.top * srcStrideBytes
                + pixelSize * srcClippedRect.left;

  for (int i = 0; i < resultHeight; i++, pdst += dstStrideBytes, psrc += srcStrideBytes) {
    memcpy(pdst, psrc, resultWidthBytes);
  }

  return true;
}

bool FrameBuffer::copyFrom(const FrameBuffer *srcFrameBuffer,
                           int srcX, int srcY)
{
  return copyFrom(&m_dimension.getRect(), srcFrameBuffer, srcX, srcY);
}

bool FrameBuffer::copyFromRotated90(const Rect *dstRect, const FrameBuffer *srcFrameBuffer,
                                    int srcX, int srcY)
{
  if (m_pixelFormat.bitsPerPixel != 32 || !m_pixelFormat.isEqualTo(&srcFrameBuffer->getPixelFormat())) {
    return false;
  }

  // Shortcuts
  int pixelSize = m_pixelFormat.bitsPerPixel / 8;
  int dstStrideBytesByX = m_dimension.width * pixelSize;
  int srcStrideBytes = srcFrameBuffer->getDimension().width * pixelSize;

  Rect srcClippedRect, dstClippedRect;

  Dimension srcBuffTransposedDim = srcFrameBuffer->getDimension().getTransposition();
  Rect srcBuffTransposedRect = srcBuffTransposedDim.getRect();
  Rect srcRotatedCoordinates(srcX, srcY, srcX + dstRect->getHeight(), srcY + dstRect->getWidth());
  srcRotatedCoordinates.rotateOn90InsideDimension(srcFrameBuffer->getDimension().height);
  int srcXinDstRotation = srcRotatedCoordinates.left;
  int srcYinDstRotation = srcRotatedCoordinates.top;
  clipRect(dstRect, &srcBuffTransposedRect, srcXinDstRotation, srcYinDstRotation, &dstClippedRect, &srcClippedRect);
  if (dstClippedRect.area() <= 0 || srcClippedRect.area() <= 0) {
    return true;
  }
  // Rotate source rect back in source rotation.
  srcClippedRect.rotateOn270InsideDimension(srcBuffTransposedDim.width);

  int resultHeight = srcClippedRect.getHeight();
  int resultWidth = srcClippedRect.getWidth();

  UINT8 *pBaseDst = (UINT8 *)m_buffer
                    + dstClippedRect.top * dstStrideBytesByX
                    + pixelSize * (dstClippedRect.right - 1);

  UINT8 *pBaseSrc = (UINT8 *)srcFrameBuffer->getBuffer()
                    + srcClippedRect.top * srcStrideBytes
                    + pixelSize * srcClippedRect.left;

  for (int iRow = 0; iRow < resultHeight; iRow++, pBaseDst -= pixelSize, pBaseSrc += srcStrideBytes) {
    UINT32 *pSrc = (UINT32 *)pBaseSrc;
    UINT8 *pDst = pBaseDst;
    for (int iCol = 0; iCol < resultWidth; iCol++, pSrc++, pDst += dstStrideBytesByX) {
      *(UINT32 *)pDst = *pSrc;
    }
  }

  return true;
}

bool FrameBuffer::copyFromRotated180(const Rect *dstRect, const FrameBuffer *srcFrameBuffer,
                                    int srcX, int srcY)
{
  if (m_pixelFormat.bitsPerPixel != 32 || !m_pixelFormat.isEqualTo(&srcFrameBuffer->getPixelFormat())) {
    return false;
  }

  // Shortcuts
  int pixelSize = m_pixelFormat.bitsPerPixel / 8;
  int dstStrideBytesByX = m_dimension.width * pixelSize;
  int srcStrideBytes = srcFrameBuffer->getDimension().width * pixelSize;

  Rect srcClippedRect, dstClippedRect;

  Dimension srcBuffTransposedDim = srcFrameBuffer->getDimension();
  Rect srcBuffTransposedRect = srcBuffTransposedDim.getRect();
  Rect srcRotatedCoordinates(srcX, srcY, srcX + dstRect->getWidth(), srcY + dstRect->getHeight());
  srcRotatedCoordinates.rotateOn180InsideDimension(srcFrameBuffer->getDimension().width,
                                                   srcFrameBuffer->getDimension().height);
  int srcXinDstRotation = srcRotatedCoordinates.left;
  int srcYinDstRotation = srcRotatedCoordinates.top;
  clipRect(dstRect, &srcBuffTransposedRect, srcXinDstRotation, srcYinDstRotation, &dstClippedRect, &srcClippedRect);
  if (dstClippedRect.area() <= 0 || srcClippedRect.area() <= 0) {
    return true;
  }
  // Rotate source rect back in source rotation.
  srcClippedRect.rotateOn180InsideDimension(srcFrameBuffer->getDimension().width,
                                            srcFrameBuffer->getDimension().height);

  int resultHeight = srcClippedRect.getHeight();
  int resultWidth = srcClippedRect.getWidth();

  UINT8 *pBaseDst = (UINT8 *)m_buffer
    + (dstClippedRect.bottom - 1) * dstStrideBytesByX
    + pixelSize * (dstClippedRect.right - 1);

  UINT8 *pBaseSrc = (UINT8 *)srcFrameBuffer->getBuffer()
    + srcClippedRect.top * srcStrideBytes
    + pixelSize * srcClippedRect.left;

  for (int iRow = 0; iRow < resultHeight; iRow++, pBaseDst -= dstStrideBytesByX , pBaseSrc += srcStrideBytes) {
    UINT32 *pSrc = (UINT32 *)pBaseSrc;
    UINT32 *pDst = (UINT32 *)pBaseDst;
    for (int iCol = 0; iCol < resultWidth; iCol++, pSrc++, pDst--) {
      *(UINT32 *)pDst = *pSrc;
    }
  }

  return true;
}

bool FrameBuffer::copyFromRotated270(const Rect *dstRect, const FrameBuffer *srcFrameBuffer,
                                    int srcX, int srcY)
{
  if (m_pixelFormat.bitsPerPixel != 32 || !m_pixelFormat.isEqualTo(&srcFrameBuffer->getPixelFormat())) {
    return false;
  }

  // Shortcuts
  int pixelSize = m_pixelFormat.bitsPerPixel / 8;
  int dstStrideBytesByX = m_dimension.width * pixelSize;
  int srcStrideBytes = srcFrameBuffer->getDimension().width * pixelSize;

  Rect srcClippedRect, dstClippedRect;

  Dimension srcBuffTransposedDim = srcFrameBuffer->getDimension().getTransposition();
  Rect srcBuffTransposedRect = srcBuffTransposedDim.getRect();
  Rect srcRotatedCoordinates(srcX, srcY, srcX + dstRect->getHeight(), srcY + dstRect->getWidth());
  srcRotatedCoordinates.rotateOn270InsideDimension(srcFrameBuffer->getDimension().width);
  int srcXinDstRotation = srcRotatedCoordinates.left;
  int srcYinDstRotation = srcRotatedCoordinates.top;
  clipRect(dstRect, &srcBuffTransposedRect, srcXinDstRotation, srcYinDstRotation, &dstClippedRect, &srcClippedRect);
  if (dstClippedRect.area() <= 0 || srcClippedRect.area() <= 0) {
    return true;
  }
  // Rotate source rect back in source rotation.
  srcClippedRect.rotateOn90InsideDimension(srcBuffTransposedDim.height);

  int resultHeight = srcClippedRect.getHeight();
  int resultWidth = srcClippedRect.getWidth();

  UINT8 *pBaseDst = (UINT8 *)m_buffer
    + (dstClippedRect.bottom - 1) * dstStrideBytesByX
    + pixelSize * dstClippedRect.left;

  UINT8 *pBaseSrc = (UINT8 *)srcFrameBuffer->getBuffer()
    + srcClippedRect.top * srcStrideBytes
    + pixelSize * srcClippedRect.left;

  for (int iRow = 0; iRow < resultHeight; iRow++, pBaseDst += pixelSize, pBaseSrc += srcStrideBytes) {
    UINT32 *pSrc = (UINT32 *)pBaseSrc;
    UINT8 *pDst = pBaseDst;
    for (int iCol = 0; iCol < resultWidth; iCol++, pSrc++, pDst -= dstStrideBytesByX) {
      *(UINT32 *)pDst = *pSrc;
    }
  }

  return true;
}

bool FrameBuffer::cmpFrom(const Rect *dstRect, const FrameBuffer *srcFrameBuffer,
                          const int srcX, const int srcY)
{
  if (!m_pixelFormat.isEqualTo(&srcFrameBuffer->getPixelFormat())) {
    return false;
  }

  Rect srcClippedRect, dstClippedRect;

  clipRect(dstRect, srcFrameBuffer, srcX, srcY, &dstClippedRect, &srcClippedRect);
  if (dstClippedRect.area() <= 0 || srcClippedRect.area() <= 0) {
    return true;
  }

  // Shortcuts
  int pixelSize = m_pixelFormat.bitsPerPixel / 8;
  int dstStrideBytes = m_dimension.width * pixelSize;
  int srcStrideBytes = srcFrameBuffer->getDimension().width * pixelSize;

  int resultHeight = dstClippedRect.getHeight();
  int resultWidthBytes = dstClippedRect.getWidth() * pixelSize;

  UINT8 *pdst = (UINT8 *)m_buffer
                + dstClippedRect.top * dstStrideBytes
                + pixelSize * dstClippedRect.left;

  UINT8 *psrc = (UINT8 *)srcFrameBuffer->getBuffer()
                + srcClippedRect.top * srcStrideBytes
                + pixelSize * srcClippedRect.left;

  for (int i = 0; i < resultHeight; i++, pdst += dstStrideBytes, psrc += srcStrideBytes) {
    if (memcmp(pdst, psrc, resultWidthBytes) != 0) {
      return false;
    }
  }

  return true;
}

void FrameBuffer::move(const Rect *dstRect, const int srcX, const int srcY)
{
  Rect srcClippedRect, dstClippedRect;

  clipRect(dstRect, this, srcX, srcY, &dstClippedRect, &srcClippedRect);
  if (dstClippedRect.area() <= 0 || srcClippedRect.area() <= 0) {
    return;
  }

  // Data copy
  int pixelSize = m_pixelFormat.bitsPerPixel / 8;
  int strideBytes = m_dimension.width * pixelSize;

  int resultHeight = dstClippedRect.getHeight();
  int resultWidthBytes = dstClippedRect.getWidth() * pixelSize;

  UINT8 *pdst, *psrc;

  if (srcY > dstRect->top) {
    // Pointers set to first string of the rectanles
    pdst = (UINT8 *)m_buffer + dstClippedRect.top * strideBytes
           + pixelSize * dstClippedRect.left;
    psrc = (UINT8 *)m_buffer + srcClippedRect.top * strideBytes
           + pixelSize * srcClippedRect.left;

    for (int i = 0; i < resultHeight; i++, pdst += strideBytes, psrc += strideBytes) {
      memcpy(pdst, psrc, resultWidthBytes);
    }

  } else {
    // Pointers set to last string of the rectanles
    pdst = (UINT8 *)m_buffer + (dstClippedRect.bottom - 1) * strideBytes
           + pixelSize * dstClippedRect.left;
    psrc = (UINT8 *)m_buffer + (srcClippedRect.bottom - 1) * strideBytes
           + pixelSize * srcClippedRect.left;

    for (int i = resultHeight - 1; i >= 0; i--, pdst -= strideBytes, psrc -= strideBytes) {
      memmove(pdst, psrc, resultWidthBytes);
    }
  }
}

bool FrameBuffer::setPixelFormat(const PixelFormat *pixelFormat)
{
  m_pixelFormat = *pixelFormat;
  return resizeBuffer();
}

bool FrameBuffer::setDimension(const Dimension *newDim)
{
  m_dimension = *newDim;
  return resizeBuffer();
}

void FrameBuffer::setEmptyDimension(const Rect *dimByRect)
{
  m_dimension.setDim(dimByRect);
}

void FrameBuffer::setEmptyPixelFmt(const PixelFormat *pf)
{
  m_pixelFormat = *pf;
}

void FrameBuffer::setPropertiesWithoutResize(const Dimension *newDim, const PixelFormat *pf)
{
  m_dimension = *newDim;
  m_pixelFormat = *pf;
}

bool FrameBuffer::setProperties(const Dimension *newDim,
                                const PixelFormat *pixelFormat)
{
  m_pixelFormat = *pixelFormat;
  m_dimension = *newDim;
  return resizeBuffer();
}

bool FrameBuffer::setProperties(const Rect *dimByRect,
                                const PixelFormat *pixelFormat)
{
  m_pixelFormat = *pixelFormat;
  m_dimension.setDim(dimByRect);
  return resizeBuffer();
}

UINT8 FrameBuffer::getBitsPerPixel() const
{
  _ASSERT((UINT8)m_pixelFormat.bitsPerPixel == m_pixelFormat.bitsPerPixel);
  return (UINT8)m_pixelFormat.bitsPerPixel;
}

UINT8 FrameBuffer::getBytesPerPixel() const
{
  return (UINT8)(m_pixelFormat.bitsPerPixel / 8);
}

void *FrameBuffer::getBufferPtr(int x, int y) const
{
  char *ptr = (char *)m_buffer;
  ptr += (y * m_dimension.width + x) * getBytesPerPixel();

  return (void *)ptr;
}

int FrameBuffer::getBufferSize() const
{ 
  return (m_dimension.area() * m_pixelFormat.bitsPerPixel) / 8;
}

bool FrameBuffer::resizeBuffer()
{
  if (m_buffer != 0) {
    delete []m_buffer;
  }
  if ((m_buffer = new UINT8[getBufferSize()]) == 0) {
    return false;
  }
  return true;
}
