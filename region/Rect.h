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

#ifndef __RECT_H__
#define __RECT_H__

#include "util/CommonHeader.h"

class Rect
{
public:
  Rect(void) : left(0), top(0), right(0), bottom(0) {}
  Rect(const Rect *rect) { setRect(rect); }
  Rect(const Rect &rect) { setRect(&rect); }
  Rect(int lt, int tp, int rt, int bm) { setRect(lt, tp, rt, bm); }
  Rect(int width, int height) { setRect(0, 0, width, height); }
  Rect(const RECT *rect) {fromWindowsRect(rect);}

  virtual ~Rect(void) {}
  
  int left;
  int top;
  int right;
  int bottom;

  inline void setRect(int lt, int tp, int rt, int bm)
  {
    left = lt;
    top = tp;
    right = rt;
    bottom = bm;
  }

  inline void setRect(const Rect *rect)
  {
    left    = rect->left;
    top     = rect->top;
    right   = rect->right;
    bottom  = rect->bottom;
  }

  inline Rect & operator=(const Rect &rect)
  {
    setRect(&rect);
    return *this;
  }

  inline bool isValid() const
  {
    if (right < left || bottom < top) {
      return false;
    }
    return true;
  }

  inline bool isPointInRect(int pointX, int pointY) {
    if (pointX < left || pointX >= right) {
      return false;
    }
    if (pointY < top || pointY >= bottom) {
      return false;
    }
    return true;
  }

  // Return true this Rect fully contain the inner Rect, otherwise return false.
  inline bool isFullyContainRect(const Rect *inner) {
    if (inner->left < left || inner->top < top ||
        inner->right > right || inner->bottom > bottom) {
      return false;
    } else {
      return true;
    }
  }

  // Convert to windows RECT
  inline RECT toWindowsRect() const {
                                RECT winRect;
                                winRect.left    = left;
                                winRect.top     = top;
                                winRect.right   = right;
                                winRect.bottom  = bottom;
                                return winRect; } const

  // Load from windows RECT
  inline void fromWindowsRect(const RECT *winRect)
  {
    left    = winRect->left;
    top     = winRect->top;
    right   = winRect->right;
    bottom  = winRect->bottom;
  }

  inline void move(int offsetX, int offsetY)
  {
    left    += offsetX;
    right   += offsetX;
    top     += offsetY;
    bottom  += offsetY;
  }

  inline void setLocation(int destX, int destY)
  {
    int offsetX = destX - left;
    int offsetY = destY - top;
    left    = destX;
    right   += offsetX;
    top     = destY;
    bottom  += offsetY;
  }

  inline bool isEqualTo(const Rect *rect)  const { return  rect->left == left &&
                                                    rect->top == top &&
                                                    rect->right == right &&
                                                    rect->bottom == bottom; }

  inline void setWidth(int value)   { right = left + value; }
  inline void setHeight(int value)  { bottom = top + value; }

  inline int getWidth()  const { return right - left; }
  inline int getHeight() const { return bottom - top; }

  void rotateOn90InsideDimension(int dimHeight)
  {
    Rect localCopy(this);
    setWidth(localCopy.getHeight());
    setHeight(localCopy.getWidth());
    int newLeft = dimHeight - localCopy.top - localCopy.getHeight();
    int newTop = localCopy.left;
    setLocation(newLeft, newTop);
  }

  void rotateOn180InsideDimension(int dimWidth, int dimHeight)
  {
    int newLeft = dimWidth - left - getWidth();
    int newTop = dimHeight - top - getHeight();
    setLocation(newLeft, newTop);
  }

  void rotateOn270InsideDimension(int dimWidth)
  {
    Rect localCopy(this);
    setWidth(localCopy.getHeight());
    setHeight(localCopy.getWidth());
    int newLeft = localCopy.top;
    int newTop = dimWidth - localCopy.left - localCopy.getWidth();
    setLocation(newLeft, newTop);
  }

  inline bool isEmpty() const { return getWidth() <= 0 || getHeight() <= 0; }
  inline int area() const { return isEmpty() ? 0 : getWidth() * getHeight(); }

  static int totalArea(std::vector<Rect> &v) {
    int area = 0;
    for (std::vector<Rect>::iterator i = v.begin(); i < v.end(); i++)
      area += i->area();
    return area;
  }

  inline void clear() { left = top = right = bottom = 0; }

  Rect intersection(const Rect *other) const {
    Rect result;
    result.setRect((left > other->left) ? left : other->left,
                   (top > other->top) ? top : other->top,
                   (right < other->right) ? right : other->right,
                   (bottom < other->bottom) ? bottom : other->bottom);
    if (!result.isValid()) {
      result.clear();
    }
    return result;
  }
};

#endif // __RECT_H__
