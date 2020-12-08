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

#include "UpdateSender.h"
#include "rfb/VendorDefs.h"
#include "rfb/EncodingDefs.h"
#include "rfb/MsgDefs.h"
#include <vector>
#include <algorithm>
#include "util/inttypes.h"
#include "util/Exception.h"
#include "UpdSenderMsgDefs.h"
#include "rfb-sconn/ClipboardExchange.h"

UpdateSender::UpdateSender(RfbCodeRegistrator *codeRegtor,
                           UpdateRequestListener *updReqListener,
                           SenderControlInformationInterface *senderControlInformation,
                           RfbOutputGate *output, int id,
                           Desktop *desktop,
                           LogWriter *log)
: m_updReqListener(updReqListener),
  m_desktop(desktop),
  m_senderControlInformation(senderControlInformation),
  m_busy(false),
  m_incrUpdIsReq(false),
  m_fullUpdIsReq(false),
  m_setColorMapEntr(false),
  m_output(output),
  m_enbox(&m_pixelConverter, m_output),
  m_id(id),
  m_videoFrozen(false),
  m_shareOnlyApp(false),
  m_log(log),
  m_cursorUpdates(log)
{
  // FIXME: argument must be defined
  m_updateKeeper = new UpdateKeeper(&Rect());

  // Capabilities
  codeRegtor->addEncCap(EncodingDefs::COPYRECT,          VendorDefs::STANDARD,
                        EncodingDefs::SIG_COPYRECT);
  codeRegtor->addEncCap(EncodingDefs::HEXTILE,           VendorDefs::STANDARD,
                        EncodingDefs::SIG_HEXTILE);
  codeRegtor->addEncCap(EncodingDefs::TIGHT,             VendorDefs::TIGHTVNC,
                        EncodingDefs::SIG_TIGHT);
  codeRegtor->addEncCap(PseudoEncDefs::COMPR_LEVEL_0,    VendorDefs::TIGHTVNC,
                        PseudoEncDefs::SIG_COMPR_LEVEL);
  codeRegtor->addEncCap(PseudoEncDefs::QUALITY_LEVEL_0,  VendorDefs::TIGHTVNC,
                        PseudoEncDefs::SIG_QUALITY_LEVEL);
  codeRegtor->addEncCap(PseudoEncDefs::RICH_CURSOR,      VendorDefs::TIGHTVNC,
                        PseudoEncDefs::SIG_RICH_CURSOR);
  codeRegtor->addEncCap(PseudoEncDefs::POINTER_POS,      VendorDefs::TIGHTVNC,
                        PseudoEncDefs::SIG_POINTER_POS);

  codeRegtor->addClToSrvCap(UpdSenderClientMsgDefs::RFB_VIDEO_FREEZE,
                            VendorDefs::TIGHTVNC,
                            UpdSenderClientMsgDefs::RFB_VIDEO_FREEZE_SIG);

  // Request codes
  codeRegtor->regCode(UpdSenderClientMsgDefs::RFB_VIDEO_FREEZE, this);
  codeRegtor->regCode(ClientMsgDefs::FB_UPDATE_REQUEST, this);
  codeRegtor->regCode(ClientMsgDefs::SET_PIXEL_FORMAT, this);
  codeRegtor->regCode(ClientMsgDefs::SET_ENCODINGS, this);

  resume();
}

UpdateSender::~UpdateSender()
{
  terminate();
  wait();
  delete m_updateKeeper;
}

void UpdateSender::onTerminate()
{
  m_newUpdatesEvent.notify();
}

void UpdateSender::onRequest(UINT32 reqCode, RfbInputGate *input)
{
  // UpdateSender internal dispatcher
  switch (reqCode) {
  case ClientMsgDefs::FB_UPDATE_REQUEST:
    readUpdateRequest(input);
    break;
  case ClientMsgDefs::SET_PIXEL_FORMAT:
    readSetPixelFormat(input);
    break;
  case ClientMsgDefs::SET_ENCODINGS:
    readSetEncodings(input);
    break;
  case UpdSenderClientMsgDefs::RFB_VIDEO_FREEZE:
    readVideoFreeze(input);
    break;
  default:
    StringStorage errMess;
    errMess.format(_T("Unknown %d protocol code received"), (int)reqCode);
    throw Exception(errMess.getString());
    break;
  }
}

void UpdateSender::init(const Dimension *viewPortDimension,
                        const PixelFormat *pf)
{
  setClientPixelFormat(pf, false);
  {
    AutoLock al(&m_viewPortMut);
    m_clientDim = *viewPortDimension;
  }
  m_lastViewPortDim = *viewPortDimension;
  m_updateKeeper->setBorderRect(&viewPortDimension->getRect());
}

void UpdateSender::newUpdates(const UpdateContainer *updateContainer,
                              const CursorShape *cursorShape)
{
  m_log->debug(_T("New updates passed to client #%d"), m_id);
  addUpdateContainer(updateContainer);

  m_cursorUpdates.updateCursorShape(cursorShape);
  {
    AutoLock al(&m_reqRectLocMut);
    m_busy = true;
    m_newUpdatesEvent.notify();
  }
  m_log->debug(_T("Client #%d is waking up"), m_id);
}

void UpdateSender::addUpdateContainer(const UpdateContainer *updateContainer)
{
  UpdateContainer updCont = *updateContainer;

  Rect viewPort = getViewPort();

  updCont.videoRegion.translate(-viewPort.left, -viewPort.top);
  updCont.changedRegion.translate(-viewPort.left, -viewPort.top);
  updCont.copiedRegion.translate(-viewPort.left, -viewPort.top);
  updCont.copySrc.move(-viewPort.left, -viewPort.top);

  m_updateKeeper->addUpdateContainer(&updCont);
}

void UpdateSender::blockCursorPosSending()
{
  m_cursorUpdates.blockCursorPosSending();
}

Rect UpdateSender::getViewPort()
{
  AutoLock al(&m_viewPortMut);
  return m_viewPort;
}

bool UpdateSender::clientIsReady()
{
  AutoLock al(&m_reqRectLocMut);
  return (m_incrUpdIsReq || m_fullUpdIsReq) && !m_busy;
}

void UpdateSender::sendRectHeader(const Rect *rect, INT32 encodingType)
{
  // FIXME: Why no warnings on passing bigger integer types?
  m_output->writeUInt16(rect->left);
  m_output->writeUInt16(rect->top);
  m_output->writeUInt16(rect->getWidth());
  m_output->writeUInt16(rect->getHeight());
  m_output->writeInt32(encodingType);
}

void UpdateSender::sendRectHeader(UINT16 x, UINT16 y, UINT16 w, UINT16 h,
                                  INT32 encodingType)
{
  m_output->writeUInt16(x);
  m_output->writeUInt16(y);
  m_output->writeUInt16(w);
  m_output->writeUInt16(h);
  m_output->writeInt32(encodingType);
}

void UpdateSender::sendNewFBSize(Dimension *dim)
{
  // Header
  m_output->writeUInt8(ServerMsgDefs::FB_UPDATE); // message type
  m_output->writeUInt8(0); // padding
  m_output->writeUInt16(1); // one rectangle

  Rect r(dim->width, dim->height);
  sendRectHeader(&r, PseudoEncDefs::DESKTOP_SIZE);
}

void UpdateSender::sendFbInClientDim(const EncodeOptions *encodeOptions,
                                     const FrameBuffer *fb,
                                     const Dimension *dim,
                                     const PixelFormat *pf)
{
  // On the black frame buffer will be overlayed the current framebuffer.
  // This is needed to combine the server frame buffer with a client frame
  // buffer when the dimensions are not equal.
  FrameBuffer blankFrameBuffer;
  blankFrameBuffer.setProperties(dim, pf);
  blankFrameBuffer.setColor(0, 0, 0);
  blankFrameBuffer.copyFrom(fb, 0, 0);

  Region region(&dim->getRect());
  std::vector<Rect> rects;
  splitRegion(m_enbox.getEncoder(), &region, &rects, &blankFrameBuffer, encodeOptions);

  // Header
  m_output->writeUInt8(0); // message type
  m_output->writeUInt8(0); // padding
  UINT16 numRects = (UINT16)rects.size();
  _ASSERT(numRects == rects.size());
  m_output->writeUInt16(numRects);
  sendRectangles(m_enbox.getEncoder(), &rects, &blankFrameBuffer, encodeOptions);
}

void UpdateSender::sendCursorShapeUpdate(const PixelFormat *fmt,
                                         const CursorShape *cursorShape)
{
  // Send pseudo-rectangle.
  Point hotSpot = cursorShape->getHotSpot();
  Dimension dim = cursorShape->getDimension();
  sendRectHeader(hotSpot.x, hotSpot.y, dim.width, dim.height,
                 PseudoEncDefs::RICH_CURSOR);

  FrameBuffer fbConverted;
  fbConverted.setProperties(&dim, fmt);
  m_pixelConverter.convert(&dim.getRect(), &fbConverted,
                           cursorShape->getPixels());

  if (fbConverted.getBufferSize()) {
    m_output->writeFully(fbConverted.getBuffer(), fbConverted.getBufferSize());
  }
  if (cursorShape->getMaskSize()) {
    m_output->writeFully(cursorShape->getMask(), cursorShape->getMaskSize());
  }
}

void UpdateSender::sendCursorPosUpdate()
{
  Point pos = m_cursorUpdates.getCurPos();
  m_log->debug(_T("Sending cursor position update: (%d,%d)"), pos.x, pos.y);
  sendRectHeader(pos.x, pos.y, 0, 0, PseudoEncDefs::POINTER_POS);
}

void UpdateSender::sendCopyRect(const std::vector<Rect> *rects, const Point *source)
{
  std::vector<Rect>::const_iterator iRect;

  for (iRect = rects->begin(); iRect != rects->end(); iRect++) {
    const Rect *rect = &(*iRect);

    sendRectHeader(rect, EncodingDefs::COPYRECT);

    // Send copyRect data
    // FIXME: Each dest rect should have own source point
    m_output->writeUInt16(source->x);
    m_output->writeUInt16(source->y);
  }
}

void UpdateSender::sendPalette(PixelFormat *pf)
{
  m_output->writeUInt8(1); // type
  m_output->writeUInt8(0); // pad
  m_output->writeUInt16(0); // first color
  m_output->writeUInt16(256); // number of colors
  for (unsigned int i = 0; i < 256; i++) {
    m_output->writeUInt16(((i >> pf->redShift) & pf->redMax) * 65535 / pf->redMax); // red
    m_output->writeUInt16(((i >> pf->greenShift) & pf->greenMax) * 65535 / pf->greenMax); // green
    m_output->writeUInt16(((i >> pf->blueShift) & pf->blueMax) * 65535 / pf->blueMax); // blue
  }
}

void UpdateSender::sendUpdate()
{
  m_log->debug(_T("Entered to the sendUpdate() function"));

//  m_log->checkPoint(_T("1 sendUpdate() begins"));

  // Check requested regions and immediately return if the client did not
  // request anything.
  Region requestedFullReg, requestedIncrReg;
  bool incrUpdIsReq, fullUpdIsReq;
  DateTime reqTimePoint;
  if (!extractReqRegions(&requestedIncrReg, &requestedFullReg,
                         &incrUpdIsReq, &fullUpdIsReq,
                         &reqTimePoint)) {
    m_log->debug(_T("No request, exiting from the sendUpdate()"));
    return;
  }
  m_log->debug(_T("Time between request and a point after extractReqRegions (in milliseconds): %u"),
    (unsigned int)(DateTime::now() - reqTimePoint).getTime());
  m_log->debug(_T("A request has been made, continuing"));
  m_log->debug(_T("The incremental region has %d rectangles"),
             (int)requestedIncrReg.getCount());
  m_log->debug(_T("The full region has %d rectangles"),
             (int)requestedFullReg.getCount());

  UpdateContainer updCont;
  extractUpdates(&updCont);

  EncodeOptions encodeOptions;
  selectEncoder(&encodeOptions);
  EncodeOptions losslessEncodeOptions;
  bool losslessEnabled = encodeOptions.jpegEnabled();
  if (losslessEnabled) {
    selectEncoder(&losslessEncodeOptions);
    losslessEncodeOptions.disableJpeg();
  }

  // Viewport calculating
  Rect viewPort;
  bool shareOnlyApp;
  Region prevShareAppRegion;
  Region shareAppRegion;
  bool viewPortChanged = updateViewPort(&viewPort, &shareOnlyApp, &prevShareAppRegion,
                                        &shareAppRegion);

  updateFrameBuffer(&updCont, shareOnlyApp, &prevShareAppRegion, &shareAppRegion);
  FrameBuffer *frameBuffer = &m_frameBuffer;

  AutoLock l(m_output);

  Dimension clientDim, lastViewPortDim;
  {
    AutoLock al(&m_viewPortMut);
    clientDim = m_clientDim;
    lastViewPortDim = m_lastViewPortDim;
  }

  // If client does not support the desktop resizing then view port dimension
  // must be no more than client dimension.
  if (!encodeOptions.desktopSizeEnabled()) {
    Rect clientRect = clientDim.getRect();
    clientRect.setLocation(viewPort.left, viewPort.top);
    viewPort = viewPort.intersection(&clientRect);
  }

  // Checking for screen size changing
  bool dimensionChanged = lastViewPortDim != Dimension(&viewPort) || updCont.screenSizeChanged;
  if (dimensionChanged) {
    updCont.screenSizeChanged = true;
  }
  if (dimensionChanged || viewPortChanged) {
    updCont.copiedRegion.clear();

    AutoLock al(&m_viewPortMut);
    m_lastViewPortDim.setDim(&viewPort);
    lastViewPortDim = m_lastViewPortDim;
    if (encodeOptions.desktopSizeEnabled()) {
      m_clientDim.setDim(&viewPort);
      clientDim = m_clientDim;
      m_updateKeeper->setBorderRect(&clientDim.getRect());
      updCont.changedRegion.crop(&clientDim.getRect());
      // Dazzle changedRegion
      updCont.changedRegion.addRect(&clientDim.getRect());
    } else {
      m_updateKeeper->setBorderRect(&lastViewPortDim.getRect());
      updCont.changedRegion.crop(&lastViewPortDim.getRect());
      // Dazzle changedRegion
      updCont.changedRegion.addRect(&lastViewPortDim.getRect());
    }
  }

  // Update pixel converter for effective pixel formats. We must do this
  // before using encoders.
  const PixelFormat serverPixelFormat = frameBuffer->getPixelFormat();
  bool setColorMapEntr;
  PixelFormat clientPixelFormat;
  {
    AutoLock lock(&m_newPixelFormatLocker);
    clientPixelFormat = m_newPixelFormat;
    setColorMapEntr = m_setColorMapEntr;
    m_setColorMapEntr = false;
  }
  if (setColorMapEntr) {
    sendPalette(&clientPixelFormat);
  }
  m_pixelConverter.setPixelFormats(&clientPixelFormat, &serverPixelFormat);

  // Send updates
  if (updCont.screenSizeChanged || (!requestedFullReg.isEmpty() &&
                                    !encodeOptions.desktopSizeEnabled())) {
    m_log->debug(_T("Screen size changed or full region requested"));
    if (encodeOptions.desktopSizeEnabled()) {
      m_log->debug(_T("Desktop resize is enabled, sending NewFBSize %dx%d"),
                 lastViewPortDim.width, lastViewPortDim.height);
      sendNewFBSize(&lastViewPortDim);
    } else {
      m_log->debug(_T("Desktop resize is disabled, sending blank screen"));
      sendFbInClientDim(&encodeOptions, frameBuffer, &clientDim,
                        &frameBuffer->getPixelFormat());
    }
    // FIXME: "Dazzle" does not seem like a good word here.
    m_log->debug(_T("Dazzle changed region"));
    m_updateKeeper->dazzleChangedReg();
  } else {
    m_log->debug(_T("Processing normal updates"));
    CursorShape cursorShape;
    m_cursorUpdates.update(&encodeOptions,
                           &updCont,
                           !requestedFullReg.isEmpty(),
                           &viewPort,
                           shareOnlyApp,
                           &shareAppRegion,
                           frameBuffer,
                           &cursorShape);

    if (!encodeOptions.copyRectEnabled() || getVideoFrozen()) {
      m_log->debug(_T("CopyRect is disabled, converting to normal updates"));
      updCont.changedRegion.add(&updCont.copiedRegion);
      updCont.copiedRegion.clear();
    }

    updCont.changedRegion.add(&m_prevVideoRegion); // This line updates rid video places when
                                                   // video is frozen.
    updCont.videoRegion.subtract(&requestedFullReg);
    updCont.changedRegion.subtract(&updCont.videoRegion);
    m_prevVideoRegion = updCont.videoRegion;
    if (getVideoFrozen()) {
      updCont.videoRegion.clear();
    }
    updCont.changedRegion.add(&requestedFullReg);

    // FIXME: Are these two lines really needed? Check that carefully.
    Rect frameBufferRect = frameBuffer->getDimension().getRect();
    updCont.videoRegion.crop(&frameBufferRect);
    updCont.changedRegion.crop(&frameBufferRect);
    shareAppRegion.crop(&frameBufferRect);
    prevShareAppRegion.crop(&frameBufferRect);
    m_losslessClean.crop(&frameBufferRect);

    // If Tight encoding is not supported by the client, convert video updates
    // to normal updates so that the preferred encoding will be used.
    if (!encodeOptions.encodingEnabled(EncodingDefs::TIGHT)) {
      updCont.changedRegion.add(&updCont.videoRegion);
      updCont.videoRegion.clear();
    }

    // Crop changed and video region by requested regions.
    cropUpdContForReqRegions(&updCont, &requestedIncrReg, &requestedFullReg);

    Region videoRegion = updCont.videoRegion;
    Region changedRegion = updCont.changedRegion;

    if (shareOnlyApp) {
      Region newOpeningAppRegion = shareAppRegion;
      newOpeningAppRegion.subtract(&prevShareAppRegion);

      Region blackRegion = changedRegion;
      blackRegion.add(&videoRegion);
      blackRegion.add(&prevShareAppRegion);
      blackRegion.subtract(&shareAppRegion);
      changedRegion.add(&blackRegion);
      changedRegion.add(&newOpeningAppRegion);
      // Paint black in the framebuffer for the black region.
      paintBlack(frameBuffer, &blackRegion);
    }

    if (losslessEnabled) {
      m_losslessDirty.add(changedRegion);
      m_losslessClean.subtract(&changedRegion);
    }
    // Add some lossless data to every update but no more than 1/100 part of framebuffer.
    // So at 20 fps full screen will be sent in 5 sec.
    int screenArea = frameBufferRect.area();
    Region losslessRegion = takePartFromRegion(&m_losslessClean, screenArea / 100);
    if (losslessEnabled) {
      if (losslessRegion.isEmpty()) {
        m_losslessClean.set(&m_losslessDirty);
        m_losslessDirty.clear();
      }
    }

    //
    // At this point, we've got final regions in changedRegion and videoRegion.
    //

    // Convert changedRegion to the final list of rectangles.
    m_log->debug(_T("Number of normal rectangles before splitting: %d"),
               changedRegion.getCount());
    std::vector<Rect> normalRects;
    splitRegion(m_enbox.getEncoder(), &changedRegion, &normalRects,
                frameBuffer, &encodeOptions);

    // Convert losslessRegion to the final list of rectangles.
    std::vector<Rect> losslessRects;
    if (losslessEnabled && !losslessRegion.isEmpty()) {
      m_log->debug(_T("Number of lossless rectangles before splitting: %d"),
        losslessRegion.getCount());
      splitRegion(m_enbox.getEncoder(), &losslessRegion, &losslessRects,
        frameBuffer, &losslessEncodeOptions);
    }
    // Do the same for the videoRegion.
    std::vector<Rect> videoRects;
    if (!videoRegion.isEmpty()) {
      m_log->debug(_T("Video region is not empty"));
      m_enbox.validateJpegEncoder(); // make sure JpegEncoder is allocated
      splitRegion(m_enbox.getJpegEncoder(), &videoRegion, &videoRects,
                  frameBuffer, &encodeOptions);
    }

    // Get the final list of CopyRect rectangles.
    std::vector<Rect> copyRects;
    updCont.copiedRegion.getRectVector(&copyRects);

    m_log->debug(_T("Number of normal rectangles: %d"), normalRects.size());
    m_log->debug(_T("Number of lossless rectangles: %d"), losslessRects.size());
    m_log->debug(_T("Number of video rectangles: %d"), videoRects.size());
    m_log->debug(_T("Number of CopyRect rectangles: %d"), copyRects.size());

    // calculate regions areas
    if (m_log->isDebug()) {
      m_log->debug(_T("Area of normal rectangles: %d"), calcAreas(normalRects));
      m_log->debug(_T("Area of lossless rectangles: %d"), calcAreas(losslessRects));
      m_log->debug(_T("Area of video rectangles: %d"), calcAreas(videoRects));
      m_log->debug(_T("Area of CopyRect rectangles: %d"), calcAreas(copyRects));
    }

    // Calculate the total number of rectangles and pseudo-rectangles.
    size_t numTotalRects =
      normalRects.size() + losslessRects.size() + videoRects.size() + copyRects.size();

    if (updCont.cursorPosChanged) {
      numTotalRects++;
      m_log->debug(_T("Adding a pseudo-rectangle for cursor position update"));
    }
    if (updCont.cursorShapeChanged) {
      numTotalRects++;
      m_log->debug(_T("Adding a pseudo-rectangle for cursor shape update"));
    }
    m_log->detail(_T("Total number of rectangles and pseudo-rectangles: %d"),
               numTotalRects);

    // FIXME: Handle this better, e.g. send first 65534 rectangles.
    _ASSERT(numTotalRects <= 65534);

    if (numTotalRects != 0) {
      m_log->debug(_T("Sending FramebufferUpdate message header"));
      // FIXME: Use constant for FramebufferUpdate message type.
      m_output->writeUInt8(0); // message type
      m_output->writeUInt8(0); // padding
      m_output->writeUInt16((UINT16)numTotalRects);

      if (updCont.cursorPosChanged) {
        sendCursorPosUpdate();
      }
      if (updCont.cursorShapeChanged) {
        m_log->debug(_T("Sending cursor shape update"));
        sendCursorShapeUpdate(&clientPixelFormat,
                              &cursorShape);
      }
      if (copyRects.size() > 0) {
        m_log->debug(_T("Sending CopyRect rectangles"));
        sendCopyRect(&copyRects, &updCont.copySrc);
      }

      m_log->debug(_T("Time between request and a point before send and coding (in milliseconds): %u"),
                 (unsigned int)(DateTime::now() - reqTimePoint).getTime());
      m_log->debug(_T("Sending video rectangles"));
      sendRectangles(m_enbox.getJpegEncoder(), &videoRects, frameBuffer, &encodeOptions);
      m_log->debug(_T("Sending normal rectangles"));
      double area = Rect::totalArea(normalRects) / 1000000.; //in millions of pixels
      ProcessorTimes pt1 = m_log->checkPoint(_T("Before Sending normal rectangles"));

      sendRectangles(m_enbox.getEncoder(), &normalRects, frameBuffer, &encodeOptions);

      sendRectangles(m_enbox.getEncoder(), &losslessRects, frameBuffer, &losslessEncodeOptions);

      ProcessorTimes pt2 = m_log->checkPoint(_T("After Sending normal rectangles"));
      m_log->debug(_T("Before Sending normal rectangles %f processor Mcycles, %f process time, %f kernel time, %f wall clock time"), 
        pt1.cycle / 1000000., pt1.process, pt1.kernel, (double)(pt1.wall.getTime()));
      double dt = (double)(pt2.wall.getTime());
      m_log->debug(_T("After Sending normal rectangles Mpoint encoded and send: %f for %f processor Mcycles"), 
        area, pt2.cycle/1000000.);
      m_log->debug(_T("After Sending normal rectangles %f process time, %f kernel time, %f wall clock time"), pt2.process, pt2.kernel, dt);

      m_log->info(_T("Time between request and answer is (in milliseconds): %u"),
                 (unsigned int)(DateTime::now() - reqTimePoint).getTime());
    } else {
      m_log->debug(_T("Nothing to send, restoring requested regions"));
      AutoLock al(&m_reqRectLocMut);
      m_requestedFullReg.add(&requestedFullReg);
      m_requestedIncrReg.add(&requestedIncrReg);
      m_incrUpdIsReq = incrUpdIsReq;
      m_fullUpdIsReq = fullUpdIsReq;
    }
    m_cursorUpdates.restoreFrameBuffer(frameBuffer);

  }

  m_log->debug(_T("Flushing output"));
//  m_log->checkPoint(_T("4 before flush"));
  m_output->flush();
//  m_log->checkPoint(_T("5 sendUpdate() end"));
}

void UpdateSender::paintBlack(FrameBuffer *frameBuffer, const Region *blackRegion)
{
  std::vector<Rect> blackRects;
  blackRegion->getRectVector(&blackRects);
  for (size_t i = 0; i < blackRects.size(); i++) {
    frameBuffer->fillRect(&blackRects[i], 0);
  }
}

void UpdateSender::splitRegion(Encoder *encoder,
                               const Region *region,
                               std::vector<Rect> *rects,
                               const FrameBuffer *frameBuffer,
                               const EncodeOptions *encodeOptions)
{
  std::vector<Rect> baseRects;
  region->getRectVector(&baseRects);
  std::vector<Rect>::iterator i;
  for (i = baseRects.begin(); i != baseRects.end(); i++) {
    encoder->splitRectangle(&*i, rects, frameBuffer, encodeOptions);
  }
}

void UpdateSender::sendRectangles(Encoder *encoder,
                                  const std::vector<Rect> *rects,
                                  const FrameBuffer *frameBuffer,
                                  const EncodeOptions *encodeOptions)
{
  std::vector<Rect>::const_iterator i;
  for (i = rects->begin(); i != rects->end(); i++) {
    sendRectHeader(&*i, encoder->getCode());
    encoder->sendRectangle(&*i, frameBuffer, encodeOptions);
  }
}

void UpdateSender::execute()
{
  m_log->info(_T("Starting update sender thread for client #%d"), m_id);

  while(!isTerminating()) {
    m_newUpdatesEvent.waitForEvent();
    {
      AutoLock al(&m_reqRectLocMut);
      m_busy = true;
    }
    m_log->debug(_T("Update sender thread of client #%d is awake"), m_id);
    if (!isTerminating()) {
      try {
        m_log->debug(_T("UpdateSender::Trying to call the sendUpdate() function"));
        sendUpdate();
        m_log->debug(_T("The sendUpdate() function has finished"));
        {
          AutoLock al(&m_reqRectLocMut);
          m_busy = false;
        }
      } catch(Exception &e) {
        m_log->interror(_T("The update sender thread caught an error and will")
                   _T(" be terminated: %s"), e.getMessage());
        Thread::terminate();
      }
    }
  }
}

void UpdateSender::readUpdateRequest(RfbInputGate *io)
{
  // Read the rest of the message:
  bool incremental = io->readUInt8() != 0;
  Rect reqRect;
  reqRect.left = io->readUInt16();
  reqRect.top = io->readUInt16();
  reqRect.setWidth(io->readUInt16());
  reqRect.setHeight(io->readUInt16());

  Region combinedReqRegions;
  {
    AutoLock al(&m_reqRectLocMut);
    if (incremental) {
      m_requestedIncrReg.addRect(&reqRect);
      m_incrUpdIsReq = true;
    } else {
      m_requestedFullReg.addRect(&reqRect);
      m_fullUpdIsReq = true;
    }
    m_requestTimePoint = DateTime::now();
    combinedReqRegions.add(&m_requestedIncrReg);
    combinedReqRegions.add(&m_requestedFullReg);
  }

  m_log->info(_T("update requested (%d, %d, %dx%d, incremental = %d)")
              _T(" by client (client #%d)"),
              reqRect.left, reqRect.top,
              reqRect.getWidth(), reqRect.getHeight(), (int)incremental,
              m_id);

  _ASSERT(m_updReqListener != 0);

  bool alreadyHasUpdates = m_updateKeeper->checkForUpdates(&combinedReqRegions);
  if (alreadyHasUpdates) {
    // We should initiaite send update to avoid it skipping on no updates from a desktop
    // FIXME: Code duplication, see the newUpdates() function.
    m_newUpdatesEvent.notify();
    m_log->debug(_T("Client #%d is waking up"), m_id);
  }

  m_updReqListener->onUpdateRequest(&reqRect, incremental);
}

void UpdateSender::readSetPixelFormat(RfbInputGate *io)
{
  PixelFormat pf;
  // Read padding
  io->readUInt16();
  io->readUInt8();

  // Read pixel format
  int bpp = io->readUInt8();
  if (bpp == 8 || bpp == 16 || bpp == 32) {
    pf.bitsPerPixel = bpp;
  } else {
    throw Exception(_T("Only 8, 16 or 32 bits per pixel supported!"));
  }
  pf.colorDepth = io->readUInt8();
  pf.bigEndian = io->readUInt8() != 0;
  bool setColorMapEntr = io->readUInt8() == 0;
  if (setColorMapEntr && bpp != 8) {
    throw Exception(_T("Only 8 bits per pixel supported with set color map ")
                    _T("entries request."));
  }
  pf.redMax = io->readUInt16();
  pf.greenMax = io->readUInt16();
  pf.blueMax = io->readUInt16();
  pf.redShift = io->readUInt8();
  pf.greenShift = io->readUInt8();
  pf.blueShift = io->readUInt8();

  // Read padding
  io->readUInt16();
  io->readUInt8();

  // If palette rewuested fill the pixel format own values
  if (setColorMapEntr) {
    pf.redMax = 7;
    pf.greenMax = 7;
    pf.blueMax = 3;
    pf.redShift = 0;
    pf.greenShift = 3;
    pf.blueShift = 6;
  }
  setClientPixelFormat(&pf, setColorMapEntr);
}

void UpdateSender::setClientPixelFormat(const PixelFormat *pf,
                                        bool clrMapEntries)
{
  AutoLock al(&m_newPixelFormatLocker);
  m_newPixelFormat = *pf;
  m_setColorMapEntr = clrMapEntries;
}

void UpdateSender::readSetEncodings(RfbInputGate *io)
{
  io->readUInt8(); // padding
  int numCodes = io->readUInt16();

  std::vector<int> list;
  list.reserve(numCodes);
  for (int i = 0; i < numCodes; i++) {
    int code = (int)io->readUInt32();
    list.push_back(code);
  }

  AutoLock lock(&m_newEncodeOptionsLocker);
  m_newEncodeOptions.setEncodings(&list);
}

void UpdateSender::setVideoFrozen(bool value)
{
  AutoLock al(&m_vidFreezeLocMut);
  m_videoFrozen = value;
}

bool UpdateSender::getVideoFrozen()
{
  AutoLock al(&m_vidFreezeLocMut);
  return m_videoFrozen;
}

void UpdateSender::readVideoFreeze(RfbInputGate *io)
{
  setVideoFrozen(io->readUInt8() != 0);
}

bool UpdateSender::extractReqRegions(Region *incrReqReg,
                                     Region *fullReqReg,
                                     bool *incrUpdIsReq,
                                     bool *fullUpdIsReq,
                                     DateTime *reqTimePoint)
{
  AutoLock al(&m_reqRectLocMut);

  *incrReqReg = m_requestedIncrReg;
  *fullReqReg = m_requestedFullReg;
  *incrUpdIsReq = m_incrUpdIsReq;
  *fullUpdIsReq = m_fullUpdIsReq;

  m_requestedFullReg.clear();
  m_requestedIncrReg.clear();
  m_incrUpdIsReq = false;
  m_fullUpdIsReq = false;

  *reqTimePoint = m_requestTimePoint;

  return *incrUpdIsReq || *fullUpdIsReq;
}

void UpdateSender::extractUpdates(UpdateContainer *updCont)
{
  m_updateKeeper->extract(updCont);
}

void UpdateSender::cropUpdContForReqRegions(UpdateContainer *updCont,
                                            const Region *incrReqReg,
                                            const Region *fullReqReg)
{
  // Crop by requested region
  Region backRegion = updCont->changedRegion;
  backRegion.add(&updCont->videoRegion);

  Region combinedReqRegion = *incrReqReg;
  combinedReqRegion.add(fullReqReg);

  inscribeCopiedRegionToReqRegion(updCont, &combinedReqRegion);

  backRegion.subtract(&combinedReqRegion);
  updCont->changedRegion.intersect(&combinedReqRegion);
  updCont->videoRegion.intersect(&combinedReqRegion);

  // Return back the out region to the update keeper.
  m_updateKeeper->addChangedRegion(&backRegion);
}

void UpdateSender::inscribeCopiedRegionToReqRegion(UpdateContainer *updCont,
                                                   const Region *requestRegion)
{
  // Test copied region. If it is fully inside in the requested region then
  // there is no to change. Otherwise, simply decline copied region.
  if (!updCont->copiedRegion.isEmpty()) {
    Region dstCopiedRegion = updCont->copiedRegion;
    dstCopiedRegion.subtract(requestRegion);

    bool copiedRegionFullyInscribed = dstCopiedRegion.isEmpty();
    if (copiedRegionFullyInscribed) {
      // Then see the same at source coordinates.
      Region copiedRegion = updCont->copiedRegion;
      Rect dstBounds = copiedRegion.getBounds();
      int dx = updCont->copySrc.x - dstBounds.left;
      int dy = updCont->copySrc.y - dstBounds.top;
      copiedRegion.translate(dx, dy);
      copiedRegion.subtract(requestRegion);
      copiedRegionFullyInscribed = copiedRegion.isEmpty();
    }
    if (!copiedRegionFullyInscribed) {
      // Convert copied region to changed region.
      updCont->changedRegion.add(&updCont->copiedRegion);
      updCont->copiedRegion.clear();
    }
  }
}

void UpdateSender::selectEncoder(EncodeOptions *encodeOptions)
{
  // Make new encode options take effect. They might have been changed on
  // receiving SetEncodings client message.
  {
    AutoLock lock(&m_newEncodeOptionsLocker);
    *encodeOptions = m_newEncodeOptions;
  }
  // Make sure the encoder object corresponds to the preferred encoding
  // requested in the most recent SetEncodings client message.
  m_enbox.selectEncoder(encodeOptions->getPreferredEncoding());
}

void UpdateSender::updateFrameBuffer(UpdateContainer *updCont,
                                     bool shareOnlyApp, const Region *prevSharedRegion,
                                     const Region *shareAppRegion)
{
  Rect viewPort = getViewPort();

  Region newOpeningPixels;
  if (shareOnlyApp) {
    updCont->changedRegion.add(&updCont->copiedRegion);
    updCont->copiedRegion.clear();
    m_appRegion = *shareAppRegion;
    newOpeningPixels = m_appRegion;
    newOpeningPixels.subtract(&m_prevAppRegion);
  }

  // Also, if shareapp checked then just opening application pixels can be already painted
  // to black and does not include to changed region (when prev pixel value = current value).
  // Thereby, new opening pixels must be updated in the framebuffer too. New opening pixels,
  // for example, appears when alien application creep on the shared application.
  updCont->changedRegion.add(&newOpeningPixels);

  // Frame buffers synchronizing
  Region changedAndCopyRgns = updCont->changedRegion;
  changedAndCopyRgns.add(&updCont->copiedRegion);
  changedAndCopyRgns.add(&updCont->videoRegion);
  changedAndCopyRgns.addRect(&m_cursorUpdates.getBackgroundRect());
  {
    AutoLock al(&m_reqRectLocMut);
    changedAndCopyRgns.add(&m_requestedFullReg);
  }

  updCont->screenSizeChanged = !m_desktop->updateExternalFrameBuffer(&m_frameBuffer, &changedAndCopyRgns, &viewPort) ||
                               updCont->screenSizeChanged;
}

bool UpdateSender::updateViewPort(Rect *outNewViewPort, bool *shareApp, Region *prevShareAppRegion,
                                  Region *newShareAppRegion)
{
  Rect newViewPort;
  m_senderControlInformation->onGetViewPort(&newViewPort, shareApp, newShareAppRegion);

  AutoLock al(&m_viewPortMut);
  bool viewPortChanged = !m_viewPort.isEqualTo(&newViewPort);
  if (viewPortChanged) {
    m_viewPort = newViewPort;
  }

  bool shareAppModeChanged = *shareApp != m_shareOnlyApp;
  // Emulating share app mode changes as view port changes.
  viewPortChanged = viewPortChanged || shareAppModeChanged;

  *outNewViewPort = newViewPort;
  return viewPortChanged;
}

// Returns part of region with total area no more than area
// and removes this part form source reg
Region UpdateSender::takePartFromRegion(Region *reg, int area)
{
  Region out;
  std::vector<Rect> rects;
  reg->getRectVector(&rects);
  // process region rects form last one, I hope it can reduce allocation number for region structure
  for (int i = rects.size() - 1; i >= 0 && area > 0; i-- ) {
    Rect r = rects[i];
    int a = r.area();
    if (a > area) {
      r.setWidth(r.getWidth() * area / a + 1);
      a = r.area();
    }
    area -= a;
    Region t(r);
    reg->subtract(&t);
    out.add(&t);
  }
  return out;
}

int UpdateSender::calcAreas(std::vector<Rect> rects)
{
  int sum = 0;
  for (int i = 0; i < rects.size(); i++) {
    sum += rects[i].area();
  }
  return sum;
}