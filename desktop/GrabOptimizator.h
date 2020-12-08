// Copyright (C) 2011,2012 GlavSoft LLC.
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

#ifndef __GRABOPTIMIZATOR_H__
#define __GRABOPTIMIZATOR_H__

#include "ScreenDriver.h"
#include "region/Region.h"
#include "util/DemandTimer.h"
#include "log-writer/LogWriter.h"
#include <vector>
#include <list>

// This class provides the screen grabbing by an optimal way.
// The class determines an optimal way by oneself dynamically.
class GrabOptimizator
{
public:
  GrabOptimizator(LogWriter *log);
  ~GrabOptimizator();

  bool grab(const Region *grabRegion, ScreenDriver *grabber);

private:
  // Returns true when the m_wholeTElements is comletly calculated.
  bool getWholeTCompleted();

  // Returns true when the m_gElements is comletly calculated.
  bool getGCompleted();

  // Returns true if all needed coefficients is calculated to use
  // the grab optimization.
  bool getOptimizationAvailable(ScreenDriver *grabber);
  bool isAvailableWholeStats();
  bool isAvailableFragmentStats();

  // Removes a part of the statistic to made it available to a refresh.
  void refreshStatistic(ScreenDriver *grabber);

  // Returns absolute sum area of rectangle vector.
  int getArea(const std::vector<Rect> *rects);

  // If the grab region is alike to whole desktop the function return true.
  bool isAlikeToWhole(const std::vector<Rect> *rects);
  bool isEnoughForWholeStats(const Rect *rect);
  // If the grab region is alike to separate fragments the function
  // return true.
  bool isAlikeToFragments(const std::vector<Rect> *rects);

  // This functions store to the log all statistic data.
  void logStatistic();

  static const size_t MIN_ELEMENTS_SIZE = 3;
  static const size_t MAX_ELEMENTS_SIZE = 10;

  __int64 grabWhole(ScreenDriver *grabber);
  __int64 grabOneRect(const Rect *rect, ScreenDriver *grabber);
  __int64 grabFragments(const std::vector<Rect> *rects, ScreenDriver *grabber);

  void addWholeTElement(double wholeT);
  void removeObsoleteWholeTElements();
  void removeFirstWholeTElement();

  void addFragmentStats(double g);
  void removeObsoleteFragmentStats();
  void removeFirstElementsFromFragmentStats();

  int m_wholeS;
  std::list<double> m_wholeTElements;
  double m_wholeTSum;

  std::list<double> m_gElements;
  double m_gSum;

  DemandTimer m_timer;

  LogWriter *m_log;
};

#endif // __GRABOPTIMIZATOR_H__
