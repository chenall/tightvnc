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

#include "DesktopServerProto.h"
#include "server-config-lib/Configurator.h"
#include "util/Exception.h"

DesktopServerProto::DesktopServerProto(BlockingGate *forwGate)
: m_forwGate(forwGate)
{
}

DesktopServerProto::~DesktopServerProto()
{
}

void DesktopServerProto::checkPixelFormat(const PixelFormat *pf)
{
  StringStorage errMess;
  if (pf->bitsPerPixel != 16 && pf->bitsPerPixel != 32) {
    errMess.format(_T("Wrong value of bits per pixel (%d)"),
                   (int)pf->bitsPerPixel);
    throw Exception(errMess.getString());
  }
  if (pf->colorDepth > pf->bitsPerPixel) {
    errMess.format(_T("Wrong value (color depth (%d) > bits per pixel (%d))"),
                   (int)pf->colorDepth, (int)pf->bitsPerPixel);
    throw Exception(errMess.getString());
  }
}

void DesktopServerProto::checkRectangle(const Rect *rect)
{
  StringStorage errMess;
  if (abs(rect->left)   > 32000 ||
      abs(rect->top)    > 32000 ||
      abs(rect->right)  > 32000 ||
      abs(rect->bottom) > 32000 ||
      !rect->isValid()) {
    errMess.format(_T("Wrong rectangle (%d, %d, %d, %d)"), rect->left,
                                                           rect->top,
                                                           rect->right,
                                                           rect->bottom);
    throw Exception(errMess.getString());
  }
}

void DesktopServerProto::checkDimension(const Dimension *dim)
{
  StringStorage errMess;
  if (abs(dim->width)  > 64000 ||
      abs(dim->height) > 64000) {
    errMess.format(_T("Wrong dimension (%dx%d)"), dim->width,
                                                  dim->height);
    throw Exception(errMess.getString());
  }
}

void DesktopServerProto::readPixelFormat(PixelFormat *pf,
                                         BlockingGate *gate)
{
  pf->bitsPerPixel = gate->readUInt16();
  pf->colorDepth   = gate->readUInt16();
  pf->redMax       = gate->readUInt16();
  pf->greenMax     = gate->readUInt16();
  pf->blueMax      = gate->readUInt16();
  pf->redShift     = gate->readUInt16();
  pf->greenShift   = gate->readUInt16();
  pf->blueShift    = gate->readUInt16();
  checkPixelFormat(pf);
}

void DesktopServerProto::sendPixelFormat(const PixelFormat *pf,
                                         BlockingGate *gate)
{
  gate->writeUInt16(pf->bitsPerPixel);
  gate->writeUInt16(pf->colorDepth);
  gate->writeUInt16(pf->redMax);
  gate->writeUInt16(pf->greenMax);
  gate->writeUInt16(pf->blueMax);
  gate->writeUInt16(pf->redShift);
  gate->writeUInt16(pf->greenShift);
  gate->writeUInt16(pf->blueShift);
}

Dimension DesktopServerProto::readDimension(BlockingGate *gate)
{
  Dimension dim;
  dim.width = gate->readInt32();
  dim.height = gate->readInt32();
  checkDimension(&dim);
  return dim;
}

void DesktopServerProto::sendDimension(const Dimension *dim,
                                       BlockingGate *gate)
{
  gate->writeInt32(dim->width);
  gate->writeInt32(dim->height);
}

Point DesktopServerProto::readPoint(BlockingGate *gate)
{
  Point point;
  point.x = gate->readInt32();
  point.y = gate->readInt32();
  return point;
}

void DesktopServerProto::sendPoint(const Point *point,
                                   BlockingGate *gate)
{
  gate->writeInt32(point->x);
  gate->writeInt32(point->y);
}

Rect DesktopServerProto::readRect(BlockingGate *gate)
{
  Rect rect;
  rect.left =  gate->readInt32();
  rect.top = gate->readInt32();
  rect.setWidth(gate->readInt32());
  rect.setHeight(gate->readInt32());

  checkRectangle(&rect);
  return rect;
}

void DesktopServerProto::sendRect(const Rect *rect,
                                  BlockingGate *gate)
{
  gate->writeInt32(rect->left);
  gate->writeInt32(rect->top);
  gate->writeInt32(rect->getWidth());
  gate->writeInt32(rect->getHeight());
}

void DesktopServerProto::sendRegion(const Region *region, BlockingGate *gate)
{
  std::vector<Rect> rects;
  std::vector<Rect>::iterator iRect;
  region->getRectVector(&rects);

  unsigned int numRects = (unsigned int)rects.size();
  _ASSERT(numRects == rects.size());
  gate->writeUInt32(numRects);

  for (iRect = rects.begin(); iRect < rects.end(); iRect++) {
    Rect *rect = &(*iRect);
    sendRect(rect, gate);
  }
}

void DesktopServerProto::readRegion(Region *region, BlockingGate *gate)
{
  region->clear();
  unsigned int rectCount = gate->readUInt32();
  for (unsigned int i = 0; i < rectCount; i++) {
    Rect r = readRect(gate);
    if (r.isValid()) {
      region->addRect(&r);
    }
  }
}

void DesktopServerProto::sendFrameBuffer(const FrameBuffer *srcFb,
                                         const Rect *srcRect,
                                         BlockingGate *gate)
{
  // FIXME: Additional FrameBuffer will be used temporarily.
  // This is easy way to send all pixels.
  PixelFormat pf = srcFb->getPixelFormat();
  FrameBuffer fb;

  fb.setProperties(srcRect, &pf);
  fb.copyFrom(srcFb, srcRect->left, srcRect->top);

  gate->writeFully(fb.getBuffer(), fb.getBufferSize());
}

void DesktopServerProto::readFrameBuffer(FrameBuffer *dstFb,
                                         const Rect *dstRect,
                                         BlockingGate *gate)
{
  // FIXME: FrameBuffer will be used temporarily.
  // This is easy way to get all pixels.
  PixelFormat pf = dstFb->getPixelFormat();
  FrameBuffer fb;
  fb.setProperties(dstRect, &pf);

  gate->readFully(fb.getBuffer(), fb.getBufferSize());
  dstFb->copyFrom(dstRect, &fb, 0, 0);
}

void DesktopServerProto::sendNewClipboard(const StringStorage *newClipboard,
                                          BlockingGate *gate)
{
  gate->writeUTF8(newClipboard->getString());
}

void DesktopServerProto::readNewClipboard(StringStorage *newClipboard,
                                          BlockingGate *gate)
{
  gate->readUTF8(newClipboard);
}

void DesktopServerProto::sendNewPointerPos(const Point *newPos, UINT8 keyFlag,
                                           BlockingGate *gate)
{
  // Send pointer position
  gate->writeUInt16(newPos->x);
  gate->writeUInt16(newPos->y);
  // Send key flags
  gate->writeUInt8(keyFlag);
}

void DesktopServerProto::readNewPointerPos(Point *newPos, UINT8 *keyFlag,
                                           BlockingGate *gate)
{
  // Read pointer position
  newPos->x = gate->readUInt16();
  newPos->y = gate->readUInt16();
  // Read key flags
  *keyFlag = gate->readUInt8();
}

void DesktopServerProto::sendKeyEvent(UINT32 keySym, bool down,
                                      BlockingGate *gate)
{
  gate->writeUInt32(keySym);
  gate->writeUInt8((UINT8)down);
}

void DesktopServerProto::readKeyEvent(UINT32 *keySym, bool *down,
                                      BlockingGate *gate)
{
  *keySym = gate->readUInt32();
  *down = gate->readUInt8() != 0;
}

void DesktopServerProto::sendUserInfo(const StringStorage *desktopName,
                                      const StringStorage *userName,
                                      BlockingGate *gate)
{
  gate->writeUTF8(desktopName->getString());
  gate->writeUTF8(userName->getString());
}

void DesktopServerProto::readUserInfo(StringStorage *desktopName,
                                      StringStorage *userName,
                                      BlockingGate *gate)
{
  gate->readUTF8(desktopName);
  gate->readUTF8(userName);
}

void DesktopServerProto::sendConfigSettings(BlockingGate *gate)
{
  ServerConfig *srvConf = Configurator::getInstance()->getServerConfig();

  // Log
  gate->writeUInt32(srvConf->getLogLevel());

  // Polling
  gate->writeUInt32(srvConf->getPollingInterval());
  gate->writeUInt8(srvConf->getGrabTransparentWindowsFlag());

  //
  gate->writeUInt8(srvConf->isBlockingLocalInput());
  gate->writeUInt8(srvConf->isLocalInputPriorityEnabled());
  gate->writeUInt32(srvConf->getLocalInputPriorityTimeout());

  gate->writeUInt8(srvConf->isRemovingDesktopWallpaperEnabled());

  // Send video class names
  AutoLock al(srvConf);
  StringVector *wndClassNames = srvConf->getVideoClassNames();
  StringVector::iterator iter = wndClassNames->begin();
  size_t stringCount = wndClassNames->size();
  gate->writeUInt32((UINT32)stringCount);
  for (; iter < wndClassNames->end(); iter++) {
    gate->writeUTF8((*iter).getString());
  }
  // Send video rects
  std::vector<Rect> *Rects = srvConf->getVideoRects();
  size_t size = Rects->size();
  gate->writeUInt32((UINT32)size);
  for (size_t i = 0; i < size; i++) {
    StringStorage s;
    RectSerializer::toString(&(Rects->at(i)), &s);
    gate->writeUTF8(s.getString());
  }
  // Send video recognition interval
  gate->writeUInt32(srvConf->getVideoRecognitionInterval());
  // Send socket timeout
  gate->writeUInt32(srvConf->getIdleTimeout());
}

void DesktopServerProto::readConfigSettings(BlockingGate *gate)
{
  ServerConfig *srvConf = Configurator::getInstance()->getServerConfig();

  // Log
  srvConf->setLogLevel(gate->readUInt32());

  // Polling
  srvConf->setPollingInterval(gate->readUInt32());
  srvConf->setGrabTransparentWindowsFlag(gate->readUInt8() != 0);

  srvConf->blockLocalInput(gate->readUInt8() != 0);
  srvConf->setLocalInputPriority(gate->readUInt8() != 0);
  srvConf->setLocalInputPriorityTimeout(gate->readUInt32());

  srvConf->enableRemovingDesktopWallpaper(gate->readUInt8() != 0);

  // Receive video class names
  AutoLock al(srvConf);
  size_t stringCount = gate->readUInt32();

  StringStorage tmpString;
  for (size_t i = 0; i < stringCount; i++) {
    gate->readUTF8(&tmpString);
    srvConf->getVideoClassNames()->push_back(tmpString);
  }
  // Receive video rects
  stringCount = gate->readUInt32();

  tmpString.setString(_T(""));
  for (size_t i = 0; i < stringCount; i++) {
    gate->readUTF8(&tmpString);
    srvConf->getVideoRects()->push_back(RectSerializer::toRect(&tmpString));
  }

  // Receive video recognition interval
  srvConf->setVideoRecognitionInterval(gate->readUInt32());
  // Receive socket timeout
  srvConf->setIdleTimeout(gate->readUInt32());
}
