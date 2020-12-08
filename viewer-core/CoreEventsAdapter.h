// Copyright (C) 2011,2012 GlavSoft LLC.
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

#ifndef _CORE_EVENTS_ADAPTER_H_
#define _CORE_EVENTS_ADAPTER_H_

#include "io-lib/DataInputStream.h"
#include "io-lib/DataOutputStream.h"
#include "io-lib/IOException.h"
#include "network/RfbOutputGate.h"
#include "rfb/FrameBuffer.h"
#include "region/Rect.h"
#include "util/Exception.h"

#include "AuthHandler.h"

//
// CoreEventsAdapter interface is used to pass events from RemoteViewerCore to
// your application.
//
// You should inherit this abstract class and override the methods you are
// interested in. For example, if all you need is to show the contents of the
// remote screen, then you should override two functions: onFrameBufferUpdate()
// and onFrameBufferPropChange().
//

class CoreEventsAdapter
{
public:
  //
  // This makes the class abstract, so that no instance can be created.
  //
  virtual ~CoreEventsAdapter() = 0;

  //
  // Bell event has been received from the server.
  //
  virtual void onBell();

  //
  // New cut text (clipboard) contents has been received from the server.
  //
  virtual void onCutText(const StringStorage *cutText);

  //
  // Connection has been established.
  //
  virtual void onEstablished();

  //
  // Protocol has entered the normal interactive phase (in other words,
  // protocol initialization has been completed).
  //
  // FIXME: document it.
  // Output need for capability, e.g. FT.
  virtual void onConnected(RfbOutputGate *output);

  //
  // RemoteViewerCore has been disconnected by calling stop()
  // or connection with server is disconnected.
  //
  // FIXME: now, onDisconnect not called after onError().
  // FIXME: change documentation or call onDisconnect() after onError().
  virtual void onDisconnect(const StringStorage *message);

  //
  // Authentication has been failed.
  // By default, onAuthError() call onError(exception).
  //
  virtual void onAuthError(const AuthException *exception);

  //
  // Error has been occured.
  //
  virtual void onError(const Exception *exception);

  // this event after update of frame buffer "fb" in rectangle "update".
  // guaranteed correct of frame buffer's area in rectangle "update".
  //
  // Frame buffer contents has been changed. During this callback,
  // the frame buffer is locked, and the rectangle is guaranteed to be valid
  // (no guarantees about other areas of the frame buffer).
  //
  virtual void onFrameBufferUpdate(const FrameBuffer *fb, const Rect *update);

  // changed properties of frame buffer.
  // In this moment frame buffer area is dirty and may be contained incorrect data
  //
  // Some properties of the frame buffer have been changed. Assume that new
  // frame buffer has been created and the old one has been destroyed. This
  // notification will be called on initial frame buffer allocation as well.
  //
  virtual void onFrameBufferPropChange(const FrameBuffer *fb);
};

#endif
