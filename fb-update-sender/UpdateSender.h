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

#ifndef __UPDATESENDER_H__
#define __UPDATESENDER_H__

#include "thread/AutoLock.h"
#include "thread/Thread.h"
#include "desktop/UpdateKeeper.h"
#include "UpdateRequestListener.h"
#include "rfb/FrameBuffer.h"
#include "ViewPort.h"
#include "network/RfbOutputGate.h"
#include "network/RfbInputGate.h"
#include "rfb-sconn/Encoder.h"
#include "rfb-sconn/HextileEncoder.h"
#include "rfb-sconn/JpegEncoder.h"
#include "rfb-sconn/EncoderStore.h"
#include "rfb-sconn/RfbCodeRegistrator.h"
#include "util/DateTime.h"
#include "CursorUpdates.h"
#include "SenderControlInformationInterface.h"
#include "log-writer/LogWriter.h"

class UpdateSender : public Thread, public RfbDispatcherListener
{
public:
  // updReqListener - pointer to the out listener for retranslate
  // update reqest to out.
  // FIXME: Document all the arguments properly.
  UpdateSender(RfbCodeRegistrator *codeRegtor,
               UpdateRequestListener *updReqListener,
               SenderControlInformationInterface *senderControlInformation,
               RfbOutputGate *output,
               int id, Desktop *desktop, LogWriter *log);
  virtual ~UpdateSender();

  // The sendServerInit() function sends first rfb init message to a client
  // FIXME: The comment does not seem to be relevant.
  void init(const Dimension *viewPortDimension, const PixelFormat *pf);

  // The triggerUpdate() function adds updateContainer to the UpdateKeeper,
  // gives new frame buffer properties if needed.
  // Also, if client is ready by now (m_reqRect is not empty), triggerUpdate()
  // notifies to thread.
  // FIXME: The comment does not seem to be relevant.
  void newUpdates(const UpdateContainer *updateContainer,
                  const CursorShape *cursorShape);

  // Block cursor pos sending by this connection to a client. Unblocking will
  // be automaticly for a time.
  void blockCursorPosSending();

  // Returns a rectangle for current ViewPort
  Rect getViewPort();

  // Check requsted regions to determine if the client is ready.
  // Return true if the client is ready, false otherwise.
  bool clientIsReady();

protected:
  // Listener function which implements RfbDispatcherListener. It will be
  // called on receiving client messages if we registered as a handler for
  // corresponding RFB message types.
  virtual void onRequest(UINT32 reqCode, RfbInputGate *input);

  // Handlers for individual RFB client messages. Called by onRequest().
  void readUpdateRequest(RfbInputGate *io);
  void readSetPixelFormat(RfbInputGate *io);
  void readSetEncodings(RfbInputGate *io);
  void readVideoFreeze(RfbInputGate *io);

  // The addUpdateContainer() function adds all updates from the first
  // updateContainer parameter to the own UpdateContainer object.
  // This function may asynchronously be called from any threads.
  void addUpdateContainer(const UpdateContainer *updateContainer);

  // The sender thread.
  virtual void execute();
  virtual void onTerminate();

  // Check cursor position for changing and store it to the m_cursorPos.
  // Return true value if cursor position has been changed.
  void checkCursorPos(UpdateContainer *updCont,
                      const Rect *viewPort);

  // Thread safed get and set of the m_videFrozen flag.
  void setVideoFrozen(bool value);
  bool getVideoFrozen();

  // The sendUpdate() function sends all stored updates to the client.
  // Access to a FrameBuffer data passes through the frameBuffer pointer
  // of the function parameter.
  void sendUpdate();

  // sendUpdate() auxiliary functions.
  // Returns true if an update has been requested.
  bool extractReqRegions(Region *incrReqReg,
                         Region *fullReqReg,
                         bool *incrUpdIsReq,
                         bool *fullUpdIsReq,
                         DateTime *reqTimePoint);
  void extractUpdates(UpdateContainer *updCont);
  void cropUpdContForReqRegions(UpdateContainer *updCont,
                                const Region *incrReqReg,
                                const Region *fullReqReg);
  void inscribeCopiedRegionToReqRegion(UpdateContainer *updCont,
                                       const Region *requestRegion);

  void selectEncoder(EncodeOptions *encodeOptions);

  // Updates pixels in the internal frame buffer.
  void updateFrameBuffer(UpdateContainer *updCont,
                         bool shareOnlyApp, const Region *prevSharedRegion,
                         const Region *shareAppRegion);
  // Updates internal view port rectangle.
  // Returns true if view port has been changed during the operation.
  bool updateViewPort(Rect *outNewViewPort, bool *shareApp, Region *prevShareAppRegion,
                      Region *newShareAppRegion);

  // The sendPalette() function sends pallete after a set color map request
  // by a client.
  void sendPalette(PixelFormat *pf);

  // Set new client pixel format. This function may be called from any thread.
  // New pixel format will take effect on sending next frame buffer update.
  void setClientPixelFormat(const PixelFormat *pf,
                            bool clrMapEntries);

  void sendRectHeader(const Rect *rect, INT32 encodingType);
  void sendRectHeader(UINT16 x, UINT16 y, UINT16 w, UINT16 h,
                      INT32 encodingType);
  void sendNewFBSize(Dimension *dim);
  void sendFbInClientDim(const EncodeOptions *encodeOptions,
                         const FrameBuffer *fb,
                         const Dimension *dim,
                         const PixelFormat *pf);
  void sendCursorShapeUpdate(const PixelFormat *fmt,
                             const CursorShape *cursorShape);
  void sendCursorPosUpdate();
  void sendCopyRect(const std::vector<Rect> *rects, const Point *source);

  // Encode and send a list of rectangles via the specified encoder.
  void sendRectangles(Encoder *encoder,
                      const std::vector<Rect> *rects,
                      const FrameBuffer *frameBuffer,
                      const EncodeOptions *encodeOptions);

  // This function paints black region in framebuffer.
  void paintBlack(FrameBuffer *frameBuffer, const Region *blackRegion);

  // This function is used to split a region into a list of rectangles,
  // where actual splitting is performed by the specified encoder object.
  // We do not use m_encoder because this function may be used for the video
  // encoder as well.
  void splitRegion(Encoder *encoder,
                   const Region *region,
                   std::vector<Rect> *rects,
                   const FrameBuffer *frameBuffer,
                   const EncodeOptions *encodeOptions);

  // Returns part of region with total area not much more than area
  // and removes this part form source reg
  Region takePartFromRegion(Region *reg, int area);
  // calculate total area of rects in pixels
  int calcAreas(std::vector<Rect> rects);

  LogWriter *m_log;

  WindowsEvent m_newUpdatesEvent;

  UpdateRequestListener *m_updReqListener;
  Region m_requestedIncrReg;
  Region m_requestedFullReg;
  bool m_incrUpdIsReq;
  bool m_fullUpdIsReq;
  bool m_busy;
  // Property for perfomance measurements. It uses with the regions mutex.
  DateTime m_requestTimePoint;
  LocalMutex m_reqRectLocMut;

  SenderControlInformationInterface *m_senderControlInformation;

  Rect m_viewPort;
  Dimension m_clientDim;
  Dimension m_lastViewPortDim;
  bool m_shareOnlyApp;
  Region m_appRegion;
  Region m_prevAppRegion;
  LocalMutex m_viewPortMut;

  UpdateKeeper *m_updateKeeper;

  FrameBuffer m_frameBuffer;
  Desktop *m_desktop;

  CursorUpdates m_cursorUpdates;

  // EncodeOptions class maintain the configuration of encoders and
  // pseudo-encoders read from the SetEncodings client message.
  // m_newEncodeOptions may be changed at any time but all change and read
  // operations must be synchronized with m_newEncodeOptionsLocker.
  // In the beginning of each framebuffer update, m_newEncodeOptions will be
  // safely copied to a local variable which then will be used while encoding
  // data. Thus, changes to m_newEncodeOptions will take effect on next
  // framebuffer update.
  EncodeOptions m_newEncodeOptions;
  LocalMutex m_newEncodeOptionsLocker;

  // Pixel format requested by the RFB client. It may be changed at any time
  // but all change and read operations must be synchronized with
  // m_newPixelFormatLocker. In the beginning of each framebuffer update, it
  // will be safely copied to a local variable which then will be used while
  // encoding data. Thus, changes to m_newPixelFormat will take effect on next
  // framebuffer update.
  PixelFormat m_newPixelFormat;
  LocalMutex m_newPixelFormatLocker;

  // This flag indicates that color map entries requested. If this flag is true
  // then before send the updates updateSender must to send the color map
  // entries
  bool m_setColorMapEntr;

  // This flag indicates that video is frozen or not.
  bool m_videoFrozen;
  // This region constains a video region which was sent at previous time.
  Region m_prevVideoRegion;
  LocalMutex m_vidFreezeLocMut;


  Region m_losslessDirty;
  Region m_losslessClean;

  // Output stream.
  RfbOutputGate *m_output;

  // PixelConverter can convert from one pixel format to another using fast
  // table lookups. It should be used only in the sender thread.
  PixelConverter m_pixelConverter;

  // All encoders are encapsulated in EncoderStore. It allocates new encoders
  // on request and maintains a pointer to the preferred encoder. This object
  // should be used only by the sender thread.
  EncoderStore m_enbox;

  // Information
  // FIXME: Document this properly.
  int m_id;
};

#endif // __UPDATESENDER_H__
