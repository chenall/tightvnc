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

#ifndef _PORT_MAPPING_CONTAINER_H_
#define _PORT_MAPPING_CONTAINER_H_

#include "PortMapping.h"

#include "util/Exception.h"

#include "io-lib/DataInputStream.h"
#include "io-lib/DataOutputStream.h"

#include <vector>

//
// Container for port mappings.
//

class PortMappingContainer
{
public:
  PortMappingContainer();
  PortMappingContainer(const PortMappingContainer &other);
  virtual ~PortMappingContainer();

  PortMappingContainer &operator=(const PortMappingContainer &other);

  // Inserts element to the end of container.
  void pushBack(PortMapping element);

  // Returns index of element or -1 if not found.
  size_t find(PortMapping searchElement) const;

  // Returns index of element with specified port.
  size_t findByPort(int port) const;

  // Removes element with specified index from container.
  void remove(size_t index);
  // Removes element from container.
  void remove(PortMapping removeMapping);

  // Removes all elements from container.
  void removeAll();

  // Returns count of elements in container.
  size_t count() const;

  // Returns true if two containers hold the same data.
  bool equals(const PortMappingContainer *other) const;

  /**
   * Serializes port all mappings in container as byte stream.
   * @param output output stream for serializing.
   * @throws Exception on error.
   */
  void serialize(DataOutputStream *output) const throw(Exception);

  /**
   * Deserializes port mapping container from input stream.
   * @param input source input stream.
   * @throws Exception on error.
   */
  void deserialize(DataInputStream *input) throw(Exception);

  // Returns a read-only pointer to the element with specifed index
  // or 0 if not found.
  const PortMapping *at(size_t index) const;

  // Returns a pointer to the element with specifed index or 0 if not found.
  PortMapping *at(size_t index);

private:
  std::vector<PortMapping> m_vector;
};

#endif
