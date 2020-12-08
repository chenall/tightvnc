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

#include "IpAccessControl.h"

void IpAccessControl::serialize(DataOutputStream *output)
{
  _ASSERT((unsigned int)size() == size());
  unsigned int count = (unsigned int)size();
  output->writeUInt32(count);

  StringStorage string;

  for (size_t i = 0; i < count; i++) {
    IpAccessRule *rule = at(i);

    rule->toString(&string);

    output->writeUTF8(string.getString());
  }
}

void IpAccessControl::deserialize(DataInputStream *input)
{
  for (iterator i = begin(); i != end(); ++i) {
    delete *i;
  }

  size_t count = input->readUInt32();
  resize(count);

  StringStorage string;

  for (iterator i = begin(); i != end(); ++i) {
    input->readUTF8(&string);

    // Here is would be good to use auto_ptr, but
    // auto_ptr is not compatible with vector.
    // FIXME: no corresponding delete. use by value?
    IpAccessRule *rule = new IpAccessRule();
    try {
      if (!IpAccessRule::parse(string.getString(), rule)) {
        throw Exception(_T("Parsing of ip access rule is failed."));
      }
    } catch (...) {
      delete rule;
      rule = 0;
      throw;
    }

    *i = rule;
  }
}
