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

#include "UpdateKeeper.h"

UpdateKeeper::UpdateKeeper()
{
}

UpdateKeeper::UpdateKeeper(const Rect *borderRect)
{
  m_borderRect.setRect(borderRect);
}

UpdateKeeper::~UpdateKeeper(void)
{
}

void UpdateKeeper::addChangedRegion(const Region *changedRegion)
{
  AutoLock al(&m_updContLocMut);

  // FIXME: Calling subtract() function is correct if use
  // copy region instead of copy rectangle.
  //m_updateContainer.copiedRegion.subtract(changedRegion);
  m_updateContainer.changedRegion.add(changedRegion);
  m_updateContainer.changedRegion.crop(&m_borderRect);
}

void UpdateKeeper::addChangedRect(const Rect *changedRect)
{
  Region region(changedRect);
  addChangedRegion(&region);
}

void UpdateKeeper::addCopyRect(const Rect *copyRect, const Point *src)
{
  AutoLock al(&m_updContLocMut);

  if (copyRect->isEmpty()) {
    return;
  }

  Region *changedRegion = &m_updateContainer.changedRegion;
  Region *copiedRegion = &m_updateContainer.copiedRegion;
  Point *copySrc = &m_updateContainer.copySrc;
  Rect dstCopyRect(copyRect);

  // Create copy of copyRect in the source coordinates.
  Rect srcCopyRect(copyRect);
  srcCopyRect.setLocation(src->x, src->y);

  // Clipping dstCopyRect
  dstCopyRect = dstCopyRect.intersection(&m_borderRect);
  // Correcting source coordinates
  srcCopyRect.left    += dstCopyRect.left - copyRect->left;
  srcCopyRect.top     += dstCopyRect.top - copyRect->top;
  srcCopyRect.right   += dstCopyRect.right - copyRect->right;
  srcCopyRect.bottom  += dstCopyRect.bottom - copyRect->bottom;
  // Clipping srcCopyRect
  Rect dummySrcCopyRect(&srcCopyRect);
  srcCopyRect = srcCopyRect.intersection(&m_borderRect);
  // Correcting destination coordinates
  dstCopyRect.left    += srcCopyRect.left - dummySrcCopyRect.left;
  dstCopyRect.top     += srcCopyRect.top - dummySrcCopyRect.top;
  dstCopyRect.right   += srcCopyRect.right - dummySrcCopyRect.right;
  dstCopyRect.bottom  += srcCopyRect.bottom - dummySrcCopyRect.bottom;

  if (dstCopyRect.isEmpty()) {
    return;
  }

  copySrc->x = srcCopyRect.left;
  copySrc->y = srcCopyRect.top;

  // Adding difference between clipped dstCopyRect and original copyRect
  // to changedRegion. Because without update detectors this information
  // loses irretrievably.
  Region diff(copyRect);
  Region dstCopyRegion(&dstCopyRect);
  diff.subtract(&dstCopyRegion);
  addChangedRegion(&diff);

  // Old copiedRegion must be added to changedRegion - (?)
  if (!copiedRegion->isEmpty()) {
    changedRegion->add(copiedRegion);
    copiedRegion->clear();
    addChangedRect(copyRect);
    return;
  }

  copiedRegion->clear();
  copiedRegion->addRect(&dstCopyRect);

  // copiedRegion must be substracted from changedRegion
  changedRegion->subtract(copiedRegion);

  // Create region that is intersection of changedRegion and srcCopyRect.
  Region addonChangedRegion(&srcCopyRect);
  addonChangedRegion.intersect(changedRegion);

  // Move addonChangedRegion and add it to changedRegion.
  addonChangedRegion.translate(dstCopyRect.left - copySrc->x,
                               dstCopyRect.top - copySrc->y);
  changedRegion->add(&addonChangedRegion);

  // Clipping regions
  m_updateContainer.changedRegion.crop(&m_borderRect);
  m_updateContainer.copiedRegion.crop(&m_borderRect);
}

void UpdateKeeper::setBorderRect(const Rect *borderRect)
{
  AutoLock al(&m_updContLocMut);
  m_borderRect = *borderRect;
}

void UpdateKeeper::setScreenSizeChanged()
{
  AutoLock al(&m_updContLocMut);
  m_updateContainer.screenSizeChanged = true;
}

void UpdateKeeper::setCursorPosChanged(const Point *curPos)
{
  AutoLock al(&m_updContLocMut);
  m_updateContainer.cursorPosChanged = true;
  m_updateContainer.cursorPos = *curPos;
}

void UpdateKeeper::setCursorPos(const Point *curPos)
{
  AutoLock al(&m_updContLocMut);
  m_updateContainer.cursorPos = *curPos;
}

void UpdateKeeper::setCursorShapeChanged()
{
  AutoLock al(&m_updContLocMut);
  m_updateContainer.cursorShapeChanged = true;
}

void UpdateKeeper::addUpdateContainer(const UpdateContainer *updateContainer)
{
  AutoLock al(&m_updContLocMut);

  // FIXME: Use addCopyRegion instead of addCopyRect
  // Add copied region
  std::vector<Rect> rects;
  std::vector<Rect>::iterator iRect;
  updateContainer->copiedRegion.getRectVector(&rects);
  size_t numRects = rects.size();
  if (numRects > 0) {
    iRect = rects.begin();
    addCopyRect(&(*iRect), &updateContainer->copySrc);
  }

  // Add changed region
  addChangedRegion(&updateContainer->changedRegion);

  // Add video region
  m_updateContainer.videoRegion.add(&updateContainer->videoRegion);

  // Set other properties
  if (updateContainer->screenSizeChanged) {
    setScreenSizeChanged();
  }
  setCursorPos(&updateContainer->cursorPos);
  if (updateContainer->cursorPosChanged) {
    setCursorPosChanged(&updateContainer->cursorPos);
  }
  if (updateContainer->cursorShapeChanged) {
    setCursorShapeChanged();
  }
}

void UpdateKeeper::getUpdateContainer(UpdateContainer *updCont)
{
  AutoLock al(&m_updContLocMut);
  *updCont = m_updateContainer;
}

bool UpdateKeeper::checkForUpdates(const Region *region)
{
  UpdateContainer updateContainer;
  getUpdateContainer(&updateContainer);

  Region resultRegion = updateContainer.changedRegion;
  resultRegion.add(&updateContainer.copiedRegion);
  resultRegion.intersect(region);

  bool result = updateContainer.cursorPosChanged ||
                updateContainer.cursorShapeChanged ||
                updateContainer.screenSizeChanged ||
                !resultRegion.isEmpty();

  return result;
}

void UpdateKeeper::extract(UpdateContainer *updateContainer)
{
  {
    AutoLock al(&m_updContLocMut);

    // Clipping regions
    m_updateContainer.changedRegion.crop(&m_borderRect);
    m_updateContainer.copiedRegion.crop(&m_borderRect);

    *updateContainer = m_updateContainer;
    m_updateContainer.clear();
  }
  {
    AutoLock al(&m_exclRegLocMut);
    updateContainer->changedRegion.subtract(&m_excludedRegion);
    updateContainer->copiedRegion.subtract(&m_excludedRegion);
  }
}

void UpdateKeeper::setExcludedRegion(const Region *excludedRegion)
{
  AutoLock al(&m_exclRegLocMut);

  if (excludedRegion == 0) {
    m_excludedRegion.clear();
  } else {
    m_excludedRegion = *excludedRegion;
  }
}
