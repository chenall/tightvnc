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

//
// TightPalette class is a container for ordered color values. Colors
// are keys in a hash where values are frequency counts. Also, there
// is a list where colors are always sorted by these counts (more
// frequent first).
//

#ifndef __RFB_TIGHTPALETTE_H_INCLUDED__
#define __RFB_TIGHTPALETTE_H_INCLUDED__

#include <string.h>
#include "util/inttypes.h"

struct TightColorList {
  TightColorList *next;
  int idx;
  UINT32 rgb;
};

struct TightPaletteEntry {
  TightColorList *listNode;
  int numPixels;
};

class TightPalette {

protected:

  // FIXME: Bigger hash table? Better hash function?
  inline static int hashFunc(UINT32 rgb) {
    return (rgb ^ (rgb >> 13)) & 0xFF;
  }

public:

  TightPalette(int maxColors = 254);

  //
  // Re-initialize the object. This does not change maximum number
  // of colors.
  //
  void reset();

  //
  // Set limit on the number of colors in the palette. Note that
  // this value cannot exceed 254.
  //
  void setMaxColors(int maxColors);

  //
  // Insert new color into the palette, or increment its counter if
  // the color is already there. Returns new number of colors, or
  // zero if the palette is full. If the palette becomes full, it
  // reports zero colors and cannot be used any more without calling
  // reset().
  //
  int insert(UINT32 rgb, int numPixels);

  //
  // Return the number of colors in the palette. If the palette is full,
  // this function returns 0.
  //
  inline int getNumColors() const {
    return m_numColors;
  }

  //
  // Return the color specified by its index in the palette.
  //
  inline UINT32 getEntry(int i) const {
    return (i < m_numColors) ? m_entry[i].listNode->rgb : (UINT32)-1;
  }

  //
  // Return the pixel counter of the color specified by its index.
  //
  inline int getCount(int i) const {
    return (i < m_numColors) ? m_entry[i].numPixels : 0;
  }

  //
  // Return the index of a specified color.
  //
  inline UINT8 getIndex(UINT32 rgb) const {
    TightColorList *pnode = m_hash[hashFunc(rgb)];
    while (pnode != NULL) {
      if (pnode->rgb == rgb) {
        return (UINT8)pnode->idx;
      }
      pnode = pnode->next;
    }
    return 0xFF;  // no such color
  }

protected:

  int m_maxColors;
  int m_numColors;

  TightPaletteEntry m_entry[256];
  TightColorList *m_hash[256];
  TightColorList m_list[256];

};

#endif // __RFB_TIGHTPALETTE_H_INCLUDED__
