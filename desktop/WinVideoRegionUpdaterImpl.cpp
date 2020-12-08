// Copyright (C) 2013 GlavSoft LLC.
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

#include "WinVideoRegionUpdaterImpl.h"
#include "server-config-lib/Configurator.h"
#include "gui/WindowFinder.h"
#include "region/RectSerializer.h"
#include "thread/AutoLock.h"

WinVideoRegionUpdaterImpl::WinVideoRegionUpdaterImpl(LogWriter *log)
  : m_log(log)
{
  resume();
}

WinVideoRegionUpdaterImpl::~WinVideoRegionUpdaterImpl()
{
  terminate();
  wait();
}

void WinVideoRegionUpdaterImpl::onTerminate()
{
  m_sleeper.notify();
}

void WinVideoRegionUpdaterImpl::execute()
{
  while (!isTerminating()) {
    m_sleeper.waitForEvent(getInterval());
    if (!isTerminating()) {
      try {
        updateVideoRegion();
      }
      catch (...) {
      }
    }
  }
}

unsigned int WinVideoRegionUpdaterImpl::getInterval() {
  ServerConfig *srvConf = Configurator::getInstance()->getServerConfig();
  return srvConf->getVideoRecognitionInterval();
}

Region WinVideoRegionUpdaterImpl::getVideoRegion()
{
  AutoLock al(&m_regionMutex);
  return m_vidRegion;
}

void WinVideoRegionUpdaterImpl::getClassNamesAndRectsFromConfig(StringVector &classNames, std::vector<Rect> &rects)
{
  ServerConfig *srvConf = Configurator::getInstance()->getServerConfig();
  AutoLock al(srvConf);
  classNames = *srvConf->getVideoClassNames();
  rects = *srvConf->getVideoRects();
}

void WinVideoRegionUpdaterImpl::updateVideoRegion()
{
  StringVector classNames;
  std::vector<Rect> rects;
  getClassNamesAndRectsFromConfig(classNames, rects);
  Region tmpRegion;
  m_log->debug(L"WinVideoRegionUpdaterImpl: ClassNames %d, Rects %d", classNames.size(), m_vidRegion.getCount());
  if (!classNames.empty()) {
    DateTime startTime = DateTime::now();
    tmpRegion.add(getRectsByClass(classNames));
    unsigned int millis = (DateTime::now() - startTime).getTime();
    m_log->debug(L"WinVideoRegionUpdaterImpl::getRectsByClass call took %d ms", millis);
  }
  if (!rects.empty()) {
    tmpRegion.add(getRectsByCoords(rects));
  }
  m_log->debug(L"WinVideoRegionUpdaterImpl: copy data");
  {
    AutoLock al(&m_regionMutex);
    m_vidRegion = tmpRegion;
  }
  m_log->debug(L"WinVideoRegionUpdaterImpl: exit updateVideoRegion()");
}

Region WinVideoRegionUpdaterImpl::getRectsByClass(StringVector classNames)
{
  std::vector<HWND> hwndVector;
  std::vector<HWND>::iterator hwndIter;
  Region vidRegion;

  for (int i = 0; i < classNames.size(); ++i) {
    m_log->debug(L"WinVideoRegionUpdaterImpl: getRectsByClass : classname: %s ", classNames[i].getString());
  }
  hwndVector = WindowFinder::findWindowsByClass(classNames);

  m_log->debug(L"WinVideoRegionUpdaterImpl: getRectsByClass : %u windows found", hwndVector.size());

  for (hwndIter = hwndVector.begin(); hwndIter != hwndVector.end(); hwndIter++) {
    HWND videoHWND = *hwndIter;
    if (videoHWND != 0) {
      WINDOWINFO wi;
      wi.cbSize = sizeof(WINDOWINFO);
      if (GetWindowInfo(videoHWND, &wi)) {
        Rect videoRect(wi.rcClient.left, wi.rcClient.top,
                       wi.rcClient.right, wi.rcClient.bottom);
        if (videoRect.isValid()) {
          videoRect.move(-GetSystemMetrics(SM_XVIRTUALSCREEN),
                         -GetSystemMetrics(SM_YVIRTUALSCREEN));
          vidRegion.addRect(&videoRect);
        }
      }
    }
  }
  return vidRegion;
}

Region WinVideoRegionUpdaterImpl::getRectsByCoords(std::vector<Rect> &rects)
{
  std::vector<Rect>::iterator rIter;
  Rect videoRect;
  Region vidRegion;
  for (rIter = rects.begin(); rIter != rects.end(); rIter++) {
    videoRect  = *rIter;
    if (videoRect.isValid()) {
      vidRegion.addRect(&videoRect);
    }
  }
  return vidRegion;
}