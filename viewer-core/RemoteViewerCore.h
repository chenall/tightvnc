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

#ifndef _REMOTE_VIEWER_CORE_H_
#define _REMOTE_VIEWER_CORE_H_

#include "log-writer/LogWriter.h"
#include "network/RfbInputGate.h"
#include "network/RfbOutputGate.h"
#include "network/socket/SocketStream.h"
#include "network/socket/SocketIPv4.h"
#include "rfb/FrameBuffer.h"
#include "region/Dimension.h"
#include "region/Point.h"
#include "thread/Thread.h"

#include "CapsContainer.h"
#include "CoreEventsAdapter.h"
#include "DispatchDataProvider.h"
#include "DecoderStore.h"
#include "FbUpdateNotifier.h"
#include "ServerMessageListener.h"
#include "TcpConnection.h"
#include "WatermarksController.h"

#include <map>
#include "UpdateRequestSender.h"

//
// RemoteViewerCore implements a local representation of a live remote screen
// updated in real time via the RFB protocol. It maintains screen data in its
// own frame buffer and notifies the target class (the adapter) about various
// events like changes in the frame buffer, incoming clipboard transfers from
// the server, protocol state changes and so on.
//
// Also, it implements remote control feature by passing arbitrary keyboard
// and pointer events to the remote computer.
//
// The interface of this class hides all the complexity of the RFB protocol
// and provides the caller with an easy way to maintain an RFB connection and
// get access to the screen data.
//
// Each function of this class may be called from any thread and from any
// number of threads simultaneously on the same object, unless documented
// otherwise. Also, each function can be called on any stage of the protocol,
// and even from callback functions of the adapter (see more about callbacks
// and the adapter below), unless documented otherwise.
//
// Finally, each function of this class may throw exceptions, unless it's
// explicitly stated that it will never do so.
//
class RemoteViewerCore : public CapabilitiesManager,
                         protected Thread
{
public:
  //
  // Dummy constructor.
  //
  // It creates an "empty" object which will not start new threads and thus
  // will not do any protocol work until start() is called. This constructor
  // should be used only when you cannot use the constructor with parameters.
  //
  // Call start() to "activate" an object created with this constructor.
  //
  // To allow logging, pass a pointer to an object which implements the Logger
  // interface (see log-writer/Logger.h). Logging is disabled by default.
  //
  RemoteViewerCore(Logger *logger = 0);

  //
  // Active constructors.
  //
  // These constructors create and initialize the object, attach it to the
  // specified data connection (e.g. an open TCP/IP socket), set callbacks for
  // events originating in the object, and start operation. To do that, they
  // call the corresponding start() function, so start() should not be called
  // explicitly for objects created with these constructors.
  //
  // See also: start() for the details on operation and threading model.
  //
  // To allow logging, pass a pointer to an object which implements the Logger
  // interface (see log-writer/Logger.h). Logging is disabled by default.
  //

  //
  // This constructor takes any type of connection that can be specified via
  // a pair of abstract "gates" (RfbInputGate and RfbOutputGate).
  //
  RemoteViewerCore(RfbInputGate *input, RfbOutputGate *output,
                   CoreEventsAdapter *adapter,
                   Logger *logger = 0,
                   bool sharedFlag = true);

  //
  // This constructor initiates new TCP connection to the specified host and
  // port, and then uses the established connection for data exchange.
  //
  // NOTE: RemoteViewerCore will not initialize Windows sockets for you.
  //       If you use this component under Windows, you should call
  //       WindowsSocket::startup() prior to calling this constructor.
  //
  RemoteViewerCore(const TCHAR *host, UINT16 port,
                   CoreEventsAdapter *adapter,
                   Logger *logger = 0,
                   bool sharedFlag = true);

  // FIX DOCUMENTATION: Clarify differents between the constructor with sockets and gates.
  //
  // This constructor expects a connected TCP socket. For example, it can be
  // used for reverse connections (when servers connect to viewers).
  //
  RemoteViewerCore(SocketIPv4 *socket,
                   CoreEventsAdapter *adapter,
                   Logger *logger = 0,
                   bool sharedFlag = true);

  // FIX DOCUMENTATION: What objects will be destroyed? Will a socket object be destroyed
  // which has been passed with the constructor?
  //
  // The destructor.
  //
  // Before actually destroying the object, it will terminate its active
  // threads and will wait until all the threads are actually finished. So
  // calling the destructor may block the calling thread until all object's
  // internal threads are gone. To prevent this, call stop() and then
  // waitTermination() prior to destroying the object.
  //
  // The destructor never throws an exception. If something goes wrong, it
  // just silently does everything possible to destroy the object cleanly, but
  // is not guaranteed to do so if any object's threads are running at the
  // moment of calling the destructor. That's another reason to call
  // stop() and waitTermination() explicitly prior to destroying the object
  // (unless you are sure there are no active threads).
  //
  virtual ~RemoteViewerCore();

  //
  // Each overloaded start() function actually starts operation.
  //
  // All start() functions attach the object to the specified data connection
  // (e.g. an open TCP/IP socket), set callbacks for events originating in the
  // object, and actually start operation.
  //
  // Three overloaded start() functions correspond to three active
  // constructors. All those constructors call the corresponding start()
  // function. All start() functions work similarly, but may do different work
  // at startup. For example, one of them initiates new TCP connection.
  //
  // These functions start a number of threads and return control to the
  // calling thread. Only one of these functions should be called, and it
  // should be called just once, and only if the object was created with the
  // dummy constructor.
  //
  // On various events, corresponding callback functions will be called via
  // the CoreEventsAdapter interface (the adapter). To receive these
  // notifications, the application should inherit CoreEventsAdapter and
  // provide a pointer to that subclass either to one of the start() functions
  // or to the constructor of this object.
  //
  // The threading model is the following. One thread created by start()
  // performs all read operations from the input gate of the data connection.
  // This thread is referred to as the "input thread". In addition to reading
  // the data, it also performs most notifications via the adapter interface,
  // except for two notifications which report changes in the frame buffer.
  //
  // The notifications related to the frame buffer, onFrameBufferUpdate() and
  // onFrameBufferPropChange(), will be called from a separate thread (let's
  // call it "frame buffer notifier"). The whole purpose of this thread is to
  // perform these two callbacks. This architecture allows input thread to
  // continue reading network data while callbacks are executed. However, note
  // that the frame buffer is locked in these two callbacks, so it's still
  // possible to block the input thread by doing too much work in the
  // callbacks -- obviously, the input thread will be blocked as soon as it
  // needs to change the frame buffer.
  //
  // Write operations (sending data to the server) may be performed from any
  // thread, both within the object and from outside of the object, assuming
  // that proper locking and flushing are used in the corresponding
  // RfbOutputGate object. Beware of sending data from callbacks, as that can
  // block the input thread for a long time.
  //

  //
  // This version of start() takes any type of connection that can be
  // specified via a pair of abstract "gates" (RfbInputGate and RfbOutputGate).
  //
  void start(RfbInputGate *input, RfbOutputGate *output,
             CoreEventsAdapter *adapter,
             bool sharedFlag = true);

  //
  // This version initiates new TCP connection to the specified host and port,
  // and then uses the established connection for data exchange.
  //
  // NOTE: RemoteViewerCore will not initialize Windows sockets for you.
  //       If you use this component under Windows, you should call
  //       WindowsSocket::startup() prior to calling function.
  //
  void start(const TCHAR *host, UINT16 port,
             CoreEventsAdapter *adapter,
             bool sharedFlag = true);

  //
  // This version expects a connected TCP socket. For example, it can be used
  // for reverse connections (when servers connect to viewers).
  //
  void start(SocketIPv4 *socket,
             CoreEventsAdapter *adapter,
             bool sharedFlag = true);

  //
  // Check if the object was started, by either calling start() or by using
  // one of the active constructors.
  //
  // If the object was started and then shut down via stop() or on error, this
  // function will still return true, as it only reports the fact that start()
  // was called, regardless of the actual state of the object.
  //
  bool wasStarted() const;

  //
  // Stop operation, detach from the data connection and terminate all active
  // threads.
  //
  // This function will return control to the caller immediately, and threads
  // may run for some time after the function has finished. To wait for actual
  // thread completion after stop(), call waitTermination().
  //
  // Note that this function does not close the actual data connection if that
  // connection was not created by this class. It will be closed only if you
  // passed host and port information either to a constructor or to start().
  //
  void stop();

  //
  // Wait until all threads have been finished. Always call this function
  // prior to destroying this object and connected objects (CoreEventsAdapter,
  // DispatchDataProvider), but only if remote viewer core was actually
  // started.
  //
  // This function does not terminate threads. That can be done by calling
  // stop(). If stop() was not and will not be called, and no error occures,
  // this function blocks the calling thread forever.
  //
  // NOTE: If remote viewer core isn't started, then function waitTermination()
  // blocks the calling thread forever.
  //
  void waitTermination();

  //
  // Informational function which returns the protocol identification string
  // in a free format. If protocol version is not known yet, an empty string
  // is returned.
  //
  StringStorage getProtocolString() const;

  //
  // Informational function which returns the remote desktop name (as received
  // from the server). If the desktop name is not available for any reason
  // (e.g. before entering the protocol phase where it will be sent), an empty
  // string is returned.
  //
  StringStorage getRemoteDesktopName() const;

  //
  // Set the specified pixel format. The viewer will request that pixel format
  // from the server, as well as a full screen update. The pixel format is not
  // guaranteed to be applied immediately. It will be applied when allowed by
  // the protocol, and adapter's onNewFrameBuffer() will be called after that.
  //
  void setPixelFormat(const PixelFormat *viewerPixelFormat);

  //
  // Enable support for Dispatcher software which acts as a proxy and connects
  // viewers with compatible servers using specially allocated ID numbers.
  // Connections to Dispatcher are initiated the same way as connections to
  // normal VNC-compatible servers, all differences between servers and
  // dispatchers will be handled automatically.
  //
  // This function must be called prior to calling start(). It should not be
  // used with constructors calling start() function implicitly. If this
  // function has not been called prior to starting the protocol, dispatched
  // connections will not be supported and protocol incompatibility error will
  // be returned.
  //
  // The argument points to an object that should implement getDispatchData()
  // function which will be called when actually connecting to a Dispatcher.
  // That function should return all necessary information for establishing
  // dispatched connection, including server's ID. See more information in the
  // DispatchDataProvider class specification.
  //
  // If 0 is passed as an argument, support for Dispatcher will be disabled.
  //
  void enableDispatching(DispatchDataProvider *src = 0);

  //
  // Pause/resume updating the frame buffer.
  //
  void stopUpdating(bool isStopped);

  //
  // Request full refresh of the framebuffer, so that the whole screen will be
  // re-sent from the server. The refresh is not guaranteed to happen
  // immediately, it will be completed when allowed by the protocol.
  //
  // This function does nothing when called before entering the main phase of
  // the protocol (when the framebuffer does not exist yet).
  //
  void refreshFrameBuffer();

  //
  // Specifies whether viewer force full update requests.
  //
  void forceFullUpdateRequests(const bool& forceUpdate);

  //
  // Defers an update requests till timeout(in milliseconds) is expired.
  // Timeout starts when a previous request is sent. But anyway the next update
  // request will not send until the response on the previous request is 
  // recieved even if timeout is expired.
  //
  void deferUpdateRequests(const int& milliseconds);

  //
  // Send a keyboard event. Arguments specify the event as defined in the
  // RFB v.3 protocol specification.
  //
  void sendKeyboardEvent(bool downFlag, UINT32 key);

  //
  // Send a pointer (mouse) event. Arguments specify the event as defined in
  // the RFB v.3 protocol specification.
  //
  void sendPointerEvent(UINT8 buttonMask, const Point *position);

  //
  // Send cut text (clipboard) to the server.
  //
  void sendCutTextEvent(const StringStorage *cutText);

  //
  // Set the preferred encoding type. Note that the server is not guaranteed
  // to use this encoding, this is only a recommendation for the server.
  // By default, Tight encoding (EncodingDefs::TIGHT) is the preferred one.
  //
  void setPreferredEncoding(INT32 encodingType);

  //
  // Allow or disallow CopyRect encoding. Correctly designed server is
  // guaranteed not to use CopyRect if disabled via this function (although
  // passing the corresponding message to the server may take time). Normally,
  // CopyRect should be enabled, and it's enabled by default.
  //
  void allowCopyRect(bool allow);

  //
  // Set JPEG image quality level for Tight encoding (in theory, it can apply
  // to other encodings if they use JPEG, but currently, only Tight encoding
  // supports that). Valid levels are in the range 0..9. Also, -1 can be used
  // to disable JPEG and ensure lossless compression (thus, it's possible to
  // say that -1 stands for best image quality).
  //
  void setJpegQualityLevel(int newJpegQualityLevel);

  //
  // Set the compression level for Tight encoging (in theory, it can apply to
  // other encodings as well, and in fact old experimental encoders such as
  // Zlib supported that, but here we do not support those encoders any more).
  // Valid levels are in the range 0..9. Also, -1 can be used to let the
  // server choose actual compression level.
  //
  void setCompressionLevel(int newCompressionLevel);

  //
  // Enable or disable cursor shape updates (enabled by default). If enabled,
  // then server sends information about the cursor shape. If disabled, cursor
  // is shown as a part of usual frame buffer updates.
  //
  void enableCursorShapeUpdates(bool enabled);

  //
  // Ignore or show cursor shape updates (shown by default). If cursor shape
  // updates are enabled but ignored, remote cursor will not be shown. This
  // option probably should not be changed unless something goes really wrong.
  //
  void ignoreCursorShapeUpdates(bool ignored);

  //
  // Enable or disable Tight security type (enabled by default). If enabled,
  // security type Tight will be selected if server supports it. 
  // If disabled, viewer does not select Tight security type even 
  // the server claims that supports it.
  //
  void enableTightSecurityType(bool enabled);
  
  //
  // Work with capabilities is documented in interface CapabilitiesManager.
  // Next methods is implements of CapabilitiesManager.
  //

  virtual void addAuthCapability(AuthHandler *authHandler,
                                 UINT32 code,
                                 const char *vendorSignature,
                                 const char *nameSignature,
                                 const StringStorage description = _T(""));

  virtual void addServerMsgCapability(ServerMessageListener *listener,
                                      UINT32 code,
                                      const char *vendorSignature,
                                      const char *nameSignature,
                                      const StringStorage description = _T(""));

  virtual void addClientMsgCapability(UINT32 code,
                                      const char *vendorSignature,
                                      const char *nameSignature,
                                      const StringStorage description = _T(""));

  virtual void addEncodingCapability(Decoder *decoder,
                                     int priorityEncoding,
                                     UINT32 code,
                                     const char *vendorSignature,
                                     const char *nameSignature,
                                     const StringStorage description = _T(""));

  virtual void getEnabledClientMsgCapabilities(vector<UINT32> *codes) const;
  virtual void getEnabledServerMsgCapabilities(vector<UINT32> *codes) const;
  virtual void getEnabledEncodingCapabilities(vector<UINT32> *codes) const;

private:
  //
  // Overrides Thread::execute(). Implements the input thread.
  //
  void execute();

  //
  // This function includes common initialization code used by constructors.
  //
  void init();

  //
  // This one includes common code reused by public start(...) functions.
  //
  void start(CoreEventsAdapter *adapter, bool sharedFlag);

  //
  // RFB protocol implementation.
  //

  //
  // Read a message type code (UINT8) from the data connection.
  // If the code is 0xFC then it's a beginning of TightVNC-specific extended
  // code so we read next 3 bytes and compose UINT32 message type.
  //
  UINT32 receiveServerMessageType();

  //
  // Read pixel format from the data connection and return it.
  //
  PixelFormat readPixelFormat();

  //
  // This method processes FramebufferUpdate server message (code 0):
  //   * receive the number of rectangles in this update,
  //   * then call receiveFrameBufferUpdRectangle() for each rectangle,
  //   * then send FramebufferUpdateRequest client message (code 3).
  //
  void receiveFbUpdate();

  //
  // Receive rectangle, decode it and notify m_fbUpdateNotifier.
  //
  // Returns true if this rectangle should be the last one in this update,
  // false otherwise. This is needed to support LastRect pseudo-encoding
  // (code -224).
  //
  bool receiveFbUpdateRectangle();

  //
  // Process a fake rectangle which represents a pseudo-encoding.
  //
  void processPseudoEncoding(const Rect *rect, int encType);

  //
  // Send FramebufferUpdateRequest client message (code 3).
  // This method updates pixel format if needed.
  //
  void sendFbUpdateRequest(bool incremental = true);

  //
  // Receive Bell server message (code 2) and send event to the adapter.
  //
  void receiveBell();

  //
  // Receive ServerCutText server message (code 3) and send event to the
  // adapter.
  //
  void receiveServerCutText();

  //
  // Receive SetColourMapEntries server message (code 1) and forget it:
  // for now, color maps are not supported.
  //
  void receiveSetColorMapEntries();

  void handleDispatcherProtocol(DispatchDataProvider *callback);

  bool isRfbProtocolString(const char protocol[12]) const;
  void connectToHost();
  void handshake();
  int negotiateAboutSecurityType();
  void authenticate();
  void clientAndServerInit();
  void readSecurityTypeList(vector<UINT32> *secTypes);
  StringStorage getSecurityTypeName(UINT32 securityType) const;
  StringStorage getAuthenticationTypeName(UINT32 authenticationType) const;
  int selectSecurityType(const vector<UINT32> *secTypes,
                         const map<UINT32, AuthHandler *> *authHandlers,
                         const bool isTightEnabled) const;
  void initTunnelling();
  int initAuthentication();
  void readCapabilities();
  RfbCapabilityInfo readCapability();
  void sendEncodings();

  //
  // This function return true, if flag m_wasConnected is true.
  // This flag is true after onConnected().
  //
  bool wasConnected() const;

  //
  // Update properties (Dimension and PixelfFormat) of m_frameBuffer.
  //
  void setFbProperties(const Dimension *fbDimension,
                       const PixelFormat *fbPixelFormat);

  //
  // If m_isNewPixelFormat flag is set to true, then pixel format of the frame buffer
  // will be updated to m_viewerPixelFormat.
  //
  // This method must be called only from the input thread, otherwise data corruption
  // is possible on the protocol level.
  //
  // Returns true if pixel format was actually changed.
  //
  bool updatePixelFormat();

  //
  // This method add pair <code, handler> to map m_authHandler.
  //
  void registerAuthHandler(const UINT32 code, AuthHandler *handler);

  //
  // This method add pair <code, listener> to map m_serverMsgHandlers.
  //
  void registerMessageListener(const UINT32 code, ServerMessageListener *listener);

  //
  // This method add pair <code, decoder> to map m_decoderHandlers.
  //
  void registerDecoderHandler(const UINT32 code, Decoder *decoder, int priority);

  LogWriter m_logWriter;

  // m_tcpConnection depends on m_logWriter and must be defined after it.
  // See also: C++ standard 12.6.2 - Initializing bases and members.
  TcpConnection m_tcpConnection;

  RfbInputGate *m_input;
  RfbOutputGate *m_output;

  CoreEventsAdapter *m_adapter;

  mutable LocalMutex m_dispatchDataProviderLock;
  DispatchDataProvider *m_dispatchDataProvider;

  WatermarksController m_watermarksController;

  // m_decoderStore depends on m_logWriter and must be defined after it.
  // See also: C++ standard 12.6.2 - Initializing bases and members.
  DecoderStore m_decoderStore;

  // m_fbUpdateNotifier depends on m_logWriter and must be defined after it.
  // See also: C++ standard 12.6.2 - Initializing bases and members.
  FbUpdateNotifier m_fbUpdateNotifier;

  CapsContainer m_authCaps;
  map<UINT32, AuthHandler *> m_authHandlers;

  CapsContainer m_clientMsgCaps;
  CapsContainer m_serverMsgCaps;
  map<UINT32, ServerMessageListener *> m_serverMsgHandlers;

  CapsContainer m_encodingCaps;
  map<UINT32, Decoder *> m_decoderHandlers;
  map<UINT32, int> m_decoderPriority;

  // This flag is set after call start().
  mutable LocalMutex m_startLock;
  bool m_wasStarted;

  // This flag is set after onConnected().
  mutable LocalMutex m_connectLock;
  bool m_wasConnected;

  // This is general frame buffer of RemoteViewerCore and local mutex to change him.
  // This frame buffer contain actual state of remote desktop.
  // Cursor painted on him before calling CoreEventsAdapter::onFrameBufferUpdate()
  // and erased after (thread FbUpdateNotifier).
  //
  // Mutex m_fbLock must locked into only this thread, else may be deadlock.
  LocalMutex m_fbLock;
  FrameBuffer m_frameBuffer;

  // Decoder work with this framebuffer. It is not actual frame buffer,
  // it only easy buffer, common to all decoders. In future this frame buffer 
  // may be replaced small buffers (e.g. 64KB) into ever decoder.
  //
  // After finish of decoding Decoder copy data to m_frameBuffer.
  // This buffer is not need to blocking: read information is one-thread.
  FrameBuffer m_rectangleFb;

  LocalMutex m_pixelFormatLock;
  bool m_isNewPixelFormat;
  PixelFormat m_viewerPixelFormat;

  LocalMutex m_refreshingLock;
  bool m_isRefreshing;

  LocalMutex m_freezeLock;
  bool m_isFreeze;

  LocalMutex m_requestUpdateLock;
  bool m_isNeedRequestUpdate;

  bool m_sharedFlag;
  int m_major;
  int m_minor;
  bool m_isTightEnabled;
  bool m_isTight;
  StringStorage m_remoteDesktopName;

  bool m_forceFullUpdate;
  
  int m_updateTimeout;

  UpdateRequestSender m_updateRequestSender;

private:
  // Do not allow copying objects.
  RemoteViewerCore(const RemoteViewerCore &);
  RemoteViewerCore &operator=(const RemoteViewerCore &);
};

#endif
