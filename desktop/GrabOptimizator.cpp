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

#include "GrabOptimizator.h"
#include "util/Exception.h"

GrabOptimizator::GrabOptimizator(LogWriter *log)
: m_gSum(0),
  m_wholeTSum(0),
  m_wholeS(0),
  m_timer(1000 * 60 * 30),
  m_log(log)
{
}

GrabOptimizator::~GrabOptimizator()
{
}

bool GrabOptimizator::grab(const Region *grabRegion, ScreenDriver *grabber)
{
  // The optimization based on the assumption: Tgrab ~= Sgrab/Vgrab + g*N,
  // where Tgrab - is grab time,
  // Sgrab - is grab area,
  // Vgrab - is a grab velocity,
  // N - is number of rectangles to grab,
  // g - overhead time costs adding on each grabbed rectangle.
  std::vector<Rect> rects;
  grabRegion->getRectVector(&rects);
  Rect boundsRect = grabRegion->getBounds();
  int boundsRectS = boundsRect.area();
  size_t n = rects.size();

  if (n == 0 || boundsRectS == 0) {
    return true;
  }

  refreshStatistic(grabber);

  if (getOptimizationAvailable(grabber)) {
    _ASSERT(m_wholeTElements.size() != 0);
    _ASSERT(m_gElements.size() != 0);

    // Get estimated time for grab by each rectangle.
    int fragS = getArea(&rects);
    double avgWholeT = (double)m_wholeTSum / m_wholeTElements.size();
    double boundsRectT = avgWholeT *
                         boundsRectS / m_wholeS; // Estimated time to
                                                 // grab by one rectangle
    double grabF = (double)avgWholeT / m_wholeS; // Grab frequency
    double avgG = m_gSum / m_gElements.size();
    double estimatedFragTime = fragS * grabF + avgG * n;
    boundsRectT += avgG;

    if (boundsRectT <= estimatedFragTime) {
      __int64 realBoundsRectTime = grabOneRect(&boundsRect, grabber);
      m_log->debug(_T("Bounds rectangle grab has been preferred:")
                 _T(" bounds rectangle time = %d,")
                 _T(" estimated fragment time = %d,")
                 _T(" estimated bounds rect time = %d"),
                 (int)realBoundsRectTime, (int)estimatedFragTime,
                 (int)boundsRectT);
      // TEST:
      //m_log->debug(_T("Test: fragment grab time = %d"),
      //           (int)grabFragments(&rects, grabber));
    } else {
      __int64 realFragTime = grabFragments(&rects, grabber);
      m_log->debug(_T("Fragment grab has been preferred:")
                 _T(" fragment time = %d, estimated fragment time = %d,")
                 _T(" estimated bounds rect time = %d"),
                 (int)realFragTime, (int)estimatedFragTime, (int)boundsRectT);
      // TEST:
      //m_log->debug(_T("Test: bounds rectangle time = %d"),
      //           (int)grabOneRect(&boundsRect, grabber));
    }
  } else {
    bool itIsEnoughForWholeStats = isEnoughForWholeStats(&boundsRect);
    bool itIsEnoughForFragmentsStats = isAlikeToFragments(&rects);

    if (itIsEnoughForWholeStats && !isAvailableWholeStats()) {
      grabOneRect(&boundsRect, grabber);

    } else if (itIsEnoughForFragmentsStats && !isAvailableFragmentStats()) {
      grabFragments(&rects, grabber);

    } else if (isAlikeToWhole(&rects)) {
      grabOneRect(&boundsRect, grabber);

    } else {
      // Grab as is
      grabFragments(&rects, grabber);
    }
  }
  return true;
}

bool GrabOptimizator::getOptimizationAvailable(ScreenDriver *grabber)
{
  int sCurrent = grabber->getScreenBuffer()->getDimension().area();
  if (m_wholeS != sCurrent) {
    // Reset all coefficients, and calculate it again.
    m_wholeS = sCurrent;
    m_wholeTSum = 0;
    m_wholeTElements.clear();

    m_gSum = 0;
    m_gElements.clear();
  }

  return isAvailableWholeStats() && isAvailableFragmentStats();
}

bool GrabOptimizator::isAvailableWholeStats()
{
  return m_wholeTElements.size() >= MIN_ELEMENTS_SIZE;
}

bool GrabOptimizator::isAvailableFragmentStats()
{
  return m_gElements.size() >= MIN_ELEMENTS_SIZE;
}

bool GrabOptimizator::getWholeTCompleted()
{
  return m_wholeTElements.size() >= MAX_ELEMENTS_SIZE;
}

bool GrabOptimizator::getGCompleted()
{
  return m_gElements.size() >= MAX_ELEMENTS_SIZE;
}

void GrabOptimizator::refreshStatistic(ScreenDriver *grabber)
{
  if (m_timer.isElapsed() && getOptimizationAvailable(grabber)) {
    m_log->debug(_T("The log statistics before refreshing:"));
    logStatistic();

    removeFirstWholeTElement();
    removeFirstElementsFromFragmentStats();

    m_timer.reset();
    m_log->debug(_T("The log statistics after refreshing:"));
    logStatistic();
  }
}

int GrabOptimizator::getArea(const std::vector<Rect> *rects)
{
  int result = 0;
  for (size_t i = 0; i < rects->size(); i++) {
    result += (*rects)[i].area();
  }
  return result;
}

bool GrabOptimizator::isAlikeToWhole(const std::vector<Rect> *rects)
{
  int area = getArea(rects);
  if (area < 1) {
    return false;
  }
  return m_wholeS / area <= 9; // area >= 10%
}

bool GrabOptimizator::isEnoughForWholeStats(const Rect *rect)
{
  int area = rect->area();
  if (area < 1) {
    return false;
  }
  return m_wholeS / rect->area() <= 9; // area >= 10%
}

bool GrabOptimizator::isAlikeToFragments(const std::vector<Rect> *rects)
{
  return rects->size() >= 10;
}

__int64 GrabOptimizator::grabWhole(ScreenDriver *grabber)
{
  // FIXME: WARNING!!! The microsoft API usage!!!
  LARGE_INTEGER timeBegin, timeEnd;
  bool timerResult1 = QueryPerformanceCounter(&timeBegin) != 0;

  if (!grabber->grabFb()) {
    throw Exception(_T("Grabber failed. Is it not ready?"));
  }

  bool timerResult2 = QueryPerformanceCounter(&timeEnd) != 0;

  if (timerResult1 && timerResult2) {
    return timeEnd.QuadPart - timeBegin.QuadPart;
  } else {
    return -1;
  }
}

__int64 GrabOptimizator::grabOneRect(const Rect *rect,
                                     ScreenDriver *grabber)
{
  int rectS = rect->area();
  _ASSERT(rectS != 0);
  // FIXME: WARNING!!! The microsoft API usage!!!
  LARGE_INTEGER timeBegin, timeEnd;
  bool timerResult1 = QueryPerformanceCounter(&timeBegin) != 0;

  if (!grabber->grabFb(rect)) {
    throw Exception(_T("Grabber failed. Is it not ready?"));
  }

  bool timerResult2 = QueryPerformanceCounter(&timeEnd) != 0;

  if (timerResult1 && timerResult2) {
    __int64 realOneRectTime = timeEnd.QuadPart - timeBegin.QuadPart;
    if (isEnoughForWholeStats(rect)) {
      // Scale the time as the whole grabbing.
      double wholeT = (double)realOneRectTime * m_wholeS / rectS;
      addWholeTElement(wholeT);
      logStatistic();
    }
    return realOneRectTime;
  } else {
    return -1;
  }
}

void GrabOptimizator::addWholeTElement(double wholeT)
{
  m_wholeTElements.push_back(wholeT);
  m_wholeTSum += wholeT;
  m_timer.reset();

  removeObsoleteWholeTElements();
}

void GrabOptimizator::removeObsoleteWholeTElements()
{
  while (m_wholeTElements.size() > MAX_ELEMENTS_SIZE) {
    removeFirstWholeTElement();
    m_timer.reset();
  }
}

void GrabOptimizator::removeFirstWholeTElement()
{
  std::list<double>::iterator iter = m_wholeTElements.begin();
  double wholeT = *iter;
  m_wholeTSum -= wholeT;
  m_wholeTElements.erase(iter);
}

__int64 GrabOptimizator::grabFragments(const std::vector<Rect> *rects,
                                       ScreenDriver *grabber)
{
  // FIXME: WARNING!!! The microsoft API usage!!!
  LARGE_INTEGER timeBegin, timeEnd;
  bool timerResult1 = QueryPerformanceCounter(&timeBegin) != 0;

  std::vector<Rect>::const_iterator iRect;
  for (iRect = rects->begin(); iRect < rects->end(); iRect++) {
    if (!grabber->grabFb(&(*iRect))) {
      throw Exception(_T("Grabber failed. Is it not ready?"));
    }
  }

  bool timerResult2 = QueryPerformanceCounter(&timeEnd) != 0;

  if (timerResult1 && timerResult2) {
    __int64 fragT = timeEnd.QuadPart - timeBegin.QuadPart;

    // To update the statistic wholeT must be calculated at least once.
    if (isAlikeToFragments(rects) &&
        m_wholeTElements.size() > 0) {
      int s = getArea(rects);
      size_t n = rects->size();
      double g = (fragT -
                  (double)m_wholeTSum / m_wholeTElements.size()
                  * s / m_wholeS) / n;
      addFragmentStats(g);

      logStatistic();
    }

    return fragT;
  } else {
    return -1;
  }
}

void GrabOptimizator::addFragmentStats(double g)
{
  m_gElements.push_back(g);
  m_gSum += g;
  m_timer.reset();

  removeObsoleteFragmentStats();
}

void GrabOptimizator::removeObsoleteFragmentStats()
{
  while (m_gElements.size() > MAX_ELEMENTS_SIZE) {
    removeFirstElementsFromFragmentStats();
    m_timer.reset();
  }
}

void GrabOptimizator::removeFirstElementsFromFragmentStats()
{
  std::list<double>::iterator iterG = m_gElements.begin();
  double g = *iterG;
  m_gSum -= g;
  m_gElements.erase(iterG);
}

void GrabOptimizator::logStatistic()
{
  m_log->debug(_T("GrabOptimizator::m_wholeS : %d"),
             m_wholeS);

  StringStorage value;
  StringStorage statString;
  for (std::list<double>::iterator iter = m_wholeTElements.begin();
       iter != m_wholeTElements.end();
       iter++) {
    value.format(_T(" %.2f;"), *iter);
    statString.appendString(value.getString());
  }
  double avgWholeT = m_wholeTElements.size() != 0 ?
                     m_wholeTSum / m_wholeTElements.size() : 0;
  m_log->debug(_T("GrabOptimizator::m_wholeT average: %.2f;")
             _T(" GrabOptimizator::m_wholeTSum: %.2f;")
             _T(" GrabOptimizator::m_wholeTElements: %s"),
             avgWholeT,
             m_wholeTSum,
             statString.getString());

  statString.setString(_T(""));
  for (std::list<double>::iterator iter = m_gElements.begin();
       iter != m_gElements.end();
       iter++) {
    value.format(_T(" %.2f;"), *iter);
    statString.appendString(value.getString());
  }
  double avgG = m_gElements.size() != 0 ?
                m_gSum / m_gElements.size() : 0;
  m_log->debug(_T("GrabOptimizator::m_g average: %.2f;")
             _T(" GrabOptimizator::m_gSum: %.2f;")
             _T(" GrabOptimizator::m_gElements: %s;"),
             avgG,
             m_gSum,
             statString.getString());
}
