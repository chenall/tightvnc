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

#ifndef __RFBINITIALIZER_H__
#define __RFBINITIALIZER_H__

#include "io-lib/Channel.h"
#include "io-lib/DataOutputStream.h"
#include "io-lib/DataInputStream.h"
#include "CapContainer.h"
#include "region/Dimension.h"
#include "rfb/PixelFormat.h"
// External listeners
#include "ClientAuthListener.h"

class RfbInitializer
{
public:
  RfbInitializer(Channel *stream,
                 ClientAuthListener *extAuthListener,
                 RfbClient *client, bool authAllowed);
  virtual ~RfbInitializer();

  void authPhase();
  void afterAuthPhase(const CapContainer *srvToClCaps,
                      const CapContainer *clToSrvCaps,
                      const CapContainer *encCaps,
                      const Dimension *dim,
                      const PixelFormat *pf);

  // Returns shared flag value. Shared flag value is valid only after
  // the authPhase() function calling.
  bool getSharedFlag() const { return m_shared; }
  bool getViewOnlyAuth() const { return m_viewOnlyAuth; }

  bool getTightEnabledFlag() const { return m_tightEnabled; }

protected:
  void initVersion();
  // @throw Exception if loopback isn't allowed.
  void checkForLoopback();
  void initAuthenticate();
  void readClientInit();
  void sendServerInit(const Dimension *dim,
                      const PixelFormat *pf);
  void sendDesktopName();
  void sendInteractionCaps(const CapContainer *srvToClCaps,
                           const CapContainer *clToSrvCaps,
                           const CapContainer *encCaps);

  void doAuth(UINT32 authType);
  void doTightAuth();
  void doVncAuth();
  void doAuthNone();

  // Calls the onCheckForBan() function by the external listener
  // @throw AuthException if current is banned.
  void checkForBan();

  // Parse RFB version string which should look like "RFB 003.008\n", check
  // that the format is valid and that the major version number is 3. Returns
  // minor version number without checking its value. If the format is invalid
  // or major version number is not 3, an Exception will be thrown.
  unsigned int getProtocolMinorVersion(const char str[12]) throw(Exception);

  DataOutputStream *m_output;
  DataInputStream *m_input;

  bool m_shared;
  unsigned int m_minorVerNum;
  bool m_viewOnlyAuth;
  bool m_tightEnabled;
  bool m_authAllowed;

  ClientAuthListener *m_extAuthListener;
  RfbClient *m_client;
};

#endif // __RFBINITIALIZER_H__
