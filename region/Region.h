// Copyright (C) 2008,2009,2010,2011,2012 GlavSoft LLC.
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

#ifndef __REGION_REGION_H_INCLUDED__
#define __REGION_REGION_H_INCLUDED__

#include <vector>
#include <list>

#include "Rect.h"

extern "C" {
#include "x11region.h"
}

/**
 * A Region is an area which can be represented by a set of rectangles with
 * integer coordinates. It maintains the list of rectangles such way that
 * individual rectangles never overlap. When a rectangle is added to a Region,
 * only its non-overlapping part will be actually added. Note that adding a
 * rectangle will not necessarily increment the number of rectangles by one.
 * On such addition, the underlying list of rectangles may change dramatically
 * and its length may increase, decrease or remain the same.
 */
class Region {
public:
  /**
   * Creates an empty region.
   */
  Region();
  /**
   * Creates a region from the given rectangle.
   * @param rect a pointer to the source rectangle.
   */
  Region(const Rect &rect);
  /**
   * Creates a copy of another region.
   * @param rect a reference to the source region.
   */
  Region(const Region &src);
  /**
   * The destructor.
   */
  virtual ~Region();

  /**
   * Clears the region so that it will consist of zero rectangles.
   */
  void clear();
  /**
   * Replaces this region with a copy of another region.
   * @param src a pointer to the source region.
   */
  void set(const Region *src);
  /**
   * Overriden assignment operator. Replaces this region with a copy of
   * another region.
   * @param src a reference to the source region.
   */
  Region & operator=(const Region &src);

  /**
   * Adds a rectangle to this region.
   * @param rect rectangle to add.
   */
  void addRect(const Rect *rect);
  /**
   * Adds offset to all rectangles in region.
   * @param dx horizontal offset to add.
   * @param dy vertical offset to add.
   */
  void translate(int dx, int dy);
  /**
   * Replaces this region by its union with another region.
   * @param other region to add.
   */
  void add(const Region *other);
  void add(const Region &other);
  /**
   * Subtracts another region from this region.
   * @param other region to subtract.
   */
  void subtract(const Region *other);
  /**
   * Replace this region by its intersection with another region.
   * @param other region to intersect with.
   */
  void intersect(const Region *other);
  /**
   * Sets this region data to intersection of this region and the specified
   * rectangle.
   * @param rect rectangle to intersect with.
   */
  void crop(const Rect *rect);

  /**
   * Checks if this region is empty.
   * @return true if this region is empty, false otherwise.
   */
  bool isEmpty() const;

  /**
   * Tests point location relative of the region.
   * @return true if this point locates inside the region, false otherwise.
   */
  bool isPointInside(int x, int y) const;

  /**
   * Checks if this region equals to another region.
   * @param other a pointer to another region to compare this region to.
   * @return true if this region equals to other, false otherwise.
   */
  bool equals(const Region *other) const;

  /**
   * Get the vector of rectangles that constitute this region.
   * @param dst pointer to an std::vector where the list of rectangles will be
   *            saved to. The previous contents of the vector will be cleared.
   */
  void getRectVector(std::vector<Rect> *dst) const;
  /**
   * Get the list of rectangles that constitute this region.
   * @param dst pointer to an std::list where the list of rectangles will be
   *            saved to. The previous contents of the list will be cleared.
   */
  void getRectList(std::list<Rect> *dst) const;

  // Returns count of rectangles in the region.
  size_t getCount() const;

  Rect getBounds() const;

private:
  /**
   * The underlying X11 region structure.
   */
  RegionRec m_reg;
};

#endif // __REGION_REGION_H_INCLUDED__
