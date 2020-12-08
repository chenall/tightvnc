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

#include "PixelConverter.h"
#include "util/inttypes.h"
#include <crtdbg.h>

PixelConverter::PixelConverter(void)
: m_convertMode(NO_CONVERT),
  m_dstFrameBuffer(0)
{
}

PixelConverter::~PixelConverter(void)
{
  reset();
}

void PixelConverter::convert(const Rect *rect, FrameBuffer *dstFb,
                             const FrameBuffer *srcFb) const
{
  if (m_convertMode == NO_CONVERT) {
    dstFb->copyFrom(rect, srcFb, rect->left, rect->top);
  } else {
    int rectHeight = rect->getHeight();
    int rectWidth = rect->getWidth();
    int fbWidth = dstFb->getDimension().width;
    PixelFormat dstPf = dstFb->getPixelFormat();
    PixelFormat srcPf = srcFb->getPixelFormat();

    UINT32 dstPixelSize = dstPf.bitsPerPixel / 8;
    UINT32 srcPixelSize = srcPf.bitsPerPixel / 8;

    // FIXME: Make FrameBuffer do the math.
    UINT8 *dstPixP = (UINT8 *)dstFb->getBuffer() +
                     (fbWidth * rect->top + rect->left) * dstPixelSize;
    UINT8 *srcPixP = (UINT8 *)srcFb->getBuffer() +
                     (fbWidth * rect->top + rect->left) * srcPixelSize;
    if (m_convertMode == CONVERT_FROM_16) {
      for (int i = 0; i < rectHeight; i++,
           dstPixP += (fbWidth - rectWidth) * dstPixelSize,
           srcPixP += (fbWidth - rectWidth) * srcPixelSize) {
        for (int j = 0; j < rectWidth; j++,
                                       dstPixP += dstPixelSize,
                                       srcPixP += srcPixelSize) {
          UINT32 dstPixel = m_hexBitsTable[*(UINT16 *)srcPixP];
          if (dstPixelSize == 4) {
            *(UINT32 *)dstPixP = (UINT32)dstPixel;
          } else if (dstPixelSize == 2) {
            *(UINT16 *)dstPixP = (UINT16)dstPixel;
          } else if (dstPixelSize == 1) {
            *(UINT8 *)dstPixP = (UINT8)dstPixel;
          }
        }
      }
    } else if (m_convertMode == CONVERT_FROM_32) {
      bool bigEndianDiffs = dstPf.bigEndian != srcPf.bigEndian;
      UINT32 srcRedMax = srcPf.redMax;
      UINT32 srcGrnMax = srcPf.greenMax;
      UINT32 srcBluMax = srcPf.blueMax;

      for (int i = 0; i < rectHeight; i++,
           dstPixP += (fbWidth - rectWidth) * dstPixelSize,
           srcPixP += (fbWidth - rectWidth) * srcPixelSize) {
        for (int j = 0; j < rectWidth; j++,
                                       dstPixP += dstPixelSize,
                                       srcPixP += srcPixelSize) {
          UINT32 dstPixel = m_redTable[*(UINT32 *)srcPixP >>
                                       srcPf.redShift & srcRedMax] |
                            m_grnTable[*(UINT32 *)srcPixP >>
                                       srcPf.greenShift & srcGrnMax] |
                            m_bluTable[*(UINT32 *)srcPixP >>
                                       srcPf.blueShift & srcBluMax];
          if (dstPixelSize == 4) {
            *(UINT32 *)dstPixP = dstPixel;
            if (bigEndianDiffs) {
              *(UINT32 *)dstPixP = rotateUint32(*(UINT32 *)dstPixP);
            }
          } else if (dstPixelSize == 2) {
            *(UINT16 *)dstPixP = dstPixel;
            if (bigEndianDiffs) {
              *(UINT16 *)dstPixP = *(UINT16 *)dstPixP << 8 |
                                   *(UINT16 *)dstPixP >> 8;
            }
          } else if (dstPixelSize == 1) {
            *(UINT8 *)dstPixP = dstPixel;
          }
        }
      } 
    } else {
      _ASSERT(0);
    }
  }
}

const FrameBuffer *
PixelConverter::convert(const Rect *rect, const FrameBuffer *srcFb)
{
  if (m_convertMode == NO_CONVERT) {
    return srcFb;
  }

  const Dimension fbSize = srcFb->getDimension();
  if (m_dstFrameBuffer == 0) {
    // No frame buffer allocated - construct new one from the scratch.
    m_dstFrameBuffer = new FrameBuffer;
    m_dstFrameBuffer->setProperties(&fbSize, &m_dstFormat);
  } else if (!m_dstFrameBuffer->getDimension().isEqualTo(&fbSize)) {
    // Frame buffer is allocated but its size it wrong - just resize it.
    // Note that if the frame buffer is allocated, its pixel format is
    // guaranteed to be relevant, because setPixelFormats() always calls
    // reset() if at least one pixel format has been changed.
    m_dstFrameBuffer->setDimension(&fbSize);
  }

  // Finally, convert pixels.
  convert(rect, m_dstFrameBuffer, srcFb);
  return m_dstFrameBuffer;
}

void PixelConverter::reset()
{
  // Deallocate the framebuffer.
  if (m_dstFrameBuffer != 0) {
    delete m_dstFrameBuffer;
    m_dstFrameBuffer = 0;
  }
}

void PixelConverter::setPixelFormats(const PixelFormat *dstPf,
                                     const PixelFormat *srcPf)
{
  if (!srcPf->isEqualTo(&m_srcFormat) || !dstPf->isEqualTo(&m_dstFormat)) {
    // Reset both translation tables and the internal frame buffer.
    reset();

    if (srcPf->isEqualTo(dstPf)) {
      m_convertMode = NO_CONVERT;
    } else if (srcPf->bitsPerPixel == 16) { // 16 bit -> N
      m_convertMode = CONVERT_FROM_16;
      fillHexBitsTable(dstPf, srcPf);
    } else if (srcPf->bitsPerPixel == 32) { // 32 bit -> N
      m_convertMode = CONVERT_FROM_32;
      fill32BitsTable(dstPf, srcPf);
    }

    m_srcFormat = *srcPf;
    m_dstFormat = *dstPf;
  }
}

size_t PixelConverter::getSrcBitsPerPixel() const
{
  return m_srcFormat.bitsPerPixel;
}

size_t PixelConverter::getDstBitsPerPixel() const
{
  return m_dstFormat.bitsPerPixel;
}

void PixelConverter::fillHexBitsTable(const PixelFormat *dstPf,
                                      const PixelFormat *srcPf)
{
  m_hexBitsTable.resize(65536);

  UINT32 dstRedMax = dstPf->redMax;
  UINT32 dstGrnMax = dstPf->greenMax;
  UINT32 dstBluMax = dstPf->blueMax;

  UINT32 dstRedShift = dstPf->redShift;
  UINT32 dstGrnShift = dstPf->greenShift;
  UINT32 dstBluShift = dstPf->blueShift;

  UINT32 srcRedMax = srcPf->redMax;
  UINT32 srcGrnMax = srcPf->greenMax;
  UINT32 srcBluMax = srcPf->blueMax;

  UINT32 srcRedMask = srcRedMax << srcPf->redShift;
  UINT32 srcGrnMask = srcGrnMax << srcPf->greenShift;
  UINT32 srcBluMask = srcBluMax << srcPf->blueShift;

  for (UINT32 i = 0; i < 65536; i++) {
    // Get source color component
    UINT32 srcRed = (i & srcRedMask) >> srcPf->redShift;
    UINT32 srcGrn = (i & srcGrnMask) >> srcPf->greenShift;
    UINT32 srcBlu = (i & srcBluMask) >> srcPf->blueShift;

    UINT32 dstRed = (srcRed * dstRedMax / srcRedMax) << dstRedShift;
    UINT32 dstGrn = (srcGrn * dstGrnMax / srcGrnMax) << dstGrnShift;
    UINT32 dstBlu = (srcBlu * dstBluMax / srcBluMax) << dstBluShift;
    m_hexBitsTable[i] = dstRed | dstGrn | dstBlu;
    if (dstPf->bigEndian != srcPf->bigEndian) {
      if (dstPf->bitsPerPixel == 32) {
        m_hexBitsTable[i] = rotateUint32(m_hexBitsTable[i]);
      }
      else if (dstPf->bitsPerPixel == 16) {
        m_hexBitsTable[i] = (m_hexBitsTable[i] & 0xff) << 8 | (m_hexBitsTable[i] & 0xff00) >> 8;
      }
    }
  }
}

void PixelConverter::fill32BitsTable(const PixelFormat *dstPf,
                                     const PixelFormat *srcPf)
{
  UINT32 dstRedMax = dstPf->redMax;
  UINT32 dstGrnMax = dstPf->greenMax;
  UINT32 dstBluMax = dstPf->blueMax;

  UINT32 dstRedShift = dstPf->redShift;
  UINT32 dstGrnShift = dstPf->greenShift;
  UINT32 dstBluShift = dstPf->blueShift;

  UINT32 srcRedMax = srcPf->redMax;
  UINT32 srcGrnMax = srcPf->greenMax;
  UINT32 srcBluMax = srcPf->blueMax;

  m_redTable.resize(srcRedMax + 1);
  m_grnTable.resize(srcGrnMax + 1);
  m_bluTable.resize(srcBluMax + 1);

  for (UINT32 i = 0; i <= srcRedMax; i++) {
    m_redTable[i] = ((i * dstRedMax + srcRedMax / 2) / srcRedMax) << dstRedShift;
  }
  for (UINT32 i = 0; i <= srcGrnMax; i++) {
    m_grnTable[i] = ((i * dstGrnMax + srcGrnMax / 2) / srcGrnMax) << dstGrnShift;
  }
  for (UINT32 i = 0; i <= srcBluMax; i++) {
    m_bluTable[i] = ((i * dstBluMax + srcBluMax / 2) / srcBluMax) << dstBluShift;
  }
}

UINT32 PixelConverter::rotateUint32(UINT32 value) const
{
  UINT32 result;
  char *src = (char *)&value;
  char *dst = (char *)&result;
  dst[0] = src[3];
  dst[1] = src[2];
  dst[2] = src[1];
  dst[3] = src[0];

  return result;
}
