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

#include "RfbCodeRegistrator.h"

RfbCodeRegistrator::RfbCodeRegistrator(RfbDispatcher *dispatcher,
                                       CapContainer *srvToClCaps,
                                       CapContainer *clToSrvCaps,
                                       CapContainer *encCaps)
: m_dispatcher(dispatcher),
  m_srvToClCaps(srvToClCaps),
  m_clToSrvCaps(clToSrvCaps),
  m_encCaps(encCaps)
{
}

RfbCodeRegistrator::~RfbCodeRegistrator()
{
}

void RfbCodeRegistrator::regCode(UINT32 code, RfbDispatcherListener *listener)
{
  m_dispatcher->registerNewHandle(code, listener);
}

void RfbCodeRegistrator::addSrvToClCap(UINT32 code, const char *vendorSignature,
                                       const char *nameSignature)
{
  m_srvToClCaps->addCap(code, vendorSignature, nameSignature);
}

void RfbCodeRegistrator::addClToSrvCap(UINT32 code, const char *vendorSignature,
                                       const char *nameSignature)
{
  m_clToSrvCaps->addCap(code, vendorSignature, nameSignature);
}

void RfbCodeRegistrator::addEncCap(UINT32 code, const char *vendorSignature,
                                   const char *nameSignature)
{
  m_encCaps->addCap(code, vendorSignature, nameSignature);
}
