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

#ifndef __CAPCONTAINER_H__
#define __CAPCONTAINER_H__

#include <vector>
#include "util/inttypes.h"
#include "io-lib/DataOutputStream.h"

// FIXME: The same structure is used in the viewer named RfbCapabilityInfo.
struct Cap
{
  UINT32 code;
  char vendorSignature[4];
  char nameSignature[8];
};

typedef std::vector<Cap> CapVector;
typedef std::vector<Cap>::const_iterator CapVectorConstIter;

class CapContainer
{
public:
  CapContainer();
  virtual ~CapContainer();

  void addCap(UINT32 code, const char *vendorSignature,
              const char *nameSignature);

  unsigned int getCapCount() const;
  void sendCaps(DataOutputStream *output) const;

  bool includes(UINT32 code) const;

private:
  CapVector m_caps;
};

#endif // __CAPCONTAINER_H__
