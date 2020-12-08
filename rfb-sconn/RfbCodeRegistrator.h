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

#ifndef __RFBCODEREGISTRATOR_H__
#define __RFBCODEREGISTRATOR_H__

#include "RfbDispatcher.h"
#include "CapContainer.h"

class RfbCodeRegistrator
{
public:
  RfbCodeRegistrator(RfbDispatcher *dispatcher,
                     CapContainer *srvToClCaps,
                     CapContainer *clToSrvCaps,
                     CapContainer *encCaps);
  virtual ~RfbCodeRegistrator();

  void regCode(UINT32 code, RfbDispatcherListener *listener);
  void addSrvToClCap(UINT32 code, const char *vendorSignature,
                     const char *nameSignature);
  void addClToSrvCap(UINT32 code, const char *vendorSignature,
                     const char *nameSignature);
  void addEncCap(UINT32 code, const char *vendorSignature,
                 const char *nameSignature);

private:
  RfbDispatcher *m_dispatcher;
  CapContainer *m_srvToClCaps;
  CapContainer *m_clToSrvCaps;
  CapContainer *m_encCaps;
};

#endif // __RFBCODEREGISTRATOR_H__
