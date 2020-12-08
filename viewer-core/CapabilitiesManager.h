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

#ifndef _CAPABILITIES_MANAGER_H_
#define _CAPABILITIES_MANAGER_H_

#include "util/CommonHeader.h"
#include "util/inttypes.h"

class AuthHandler;
class ServerMessageListener;
class Decoder;

//
// This interface defined function, than need for work with capabilities.
// Any capabilities must be added before call RemoteViewerCore::start().
//
class CapabilitiesManager
{
public:
  CapabilitiesManager();
  virtual ~CapabilitiesManager();

  //
  // This function add authentication capability and authentication handler.
  //
  virtual void addAuthCapability(AuthHandler *authHandler,
                                 UINT32 code,
                                 const char *vendorSignature,
                                 const char *nameSignature,
                                 const StringStorage description = _T("")) = 0;

  //
  // This function add capability and handler, to process server-to-client messages.
  //
  virtual void addServerMsgCapability(ServerMessageListener *listener,
                                      UINT32 code,
                                      const char *vendorSignature,
                                      const char *nameSignature,
                                      const StringStorage description = _T("")) = 0;

  //
  // This function add capability, to process client-to-server messages.
  //
  virtual void addClientMsgCapability(UINT32 code,
                                      const char *vendorSignature,
                                      const char *nameSignature,
                                      const StringStorage description = _T("")) = 0;

  //
  // This function add encoding capability and decoder handler.
  // Parameter "priorityEncoding" define priority of encoding.
  // Encoding send to order in decrease of priority.
  //
  virtual void addEncodingCapability(Decoder *decoder,
                                     int priorityEncoding,
                                     UINT32 code,
                                     const char *vendorSignature,
                                     const char *nameSignature,
                                     const StringStorage description = _T("")) = 0;

  //
  // This function return vector of enabled capabilities "client-to-server message".
  //
  virtual void getEnabledClientMsgCapabilities(vector<UINT32> *codes) const = 0;

  //
  // This function return vector of enabled capabilities "server-to-client message".
  //
  virtual void getEnabledServerMsgCapabilities(vector<UINT32> *codes) const = 0;

  //
  // This function return vector of enabled capabilities "encoding".
  //
  virtual void getEnabledEncodingCapabilities(vector<UINT32> *codes) const = 0;
};

#endif
