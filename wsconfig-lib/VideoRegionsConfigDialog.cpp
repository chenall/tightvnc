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

#include "tvnserver/resource.h"
#include "VideoRegionsConfigDialog.h"
#include "ConfigDialog.h"
#include "UIDataAccess.h"
#include "CommonInputValidation.h"
#include "util/StringParser.h"

VideoRegionsConfigDialog::VideoRegionsConfigDialog()
: BaseDialog(IDD_CONFIG_VIDEO_PAGE), m_parentDialog(NULL)
{
}

VideoRegionsConfigDialog::~VideoRegionsConfigDialog()
{
}

void VideoRegionsConfigDialog::setParentDialog(BaseDialog *dialog)
{
  m_parentDialog = dialog;
}

BOOL VideoRegionsConfigDialog::onInitDialog()
{
  m_config = Configurator::getInstance()->getServerConfig();
  initControls();
  updateUI();
  return TRUE;
}

BOOL VideoRegionsConfigDialog::onNotify(UINT controlID, LPARAM data)
{
  if (controlID == IDC_VIDEO_RECOGNITION_INTERVAL_SPIN) {
    LPNMUPDOWN message = (LPNMUPDOWN)data;
    if (message->hdr.code == UDN_DELTAPOS) {
      onRecognitionIntervalSpinChangePos(message);
    }
  }
  return TRUE;
}

BOOL VideoRegionsConfigDialog::onCommand(UINT controlID, UINT notificationID)
{
  if (notificationID == EN_UPDATE) {
    if (controlID == IDC_VIDEO_CLASS_NAMES || controlID == IDC_VIDEO_RECTS) {
      onVideoRegionsUpdate();
    } else if (controlID == IDC_VIDEO_RECOGNITION_INTERVAL) {
      onRecognitionIntervalUpdate();
    } 
  }
  return TRUE;
}

bool VideoRegionsConfigDialog::validateInput()
{
  if (!CommonInputValidation::validateUINT(
    &m_videoRecognitionInterval,
    StringTable::getString(IDS_INVALID_VIDEO_RECOGNITION_INTERVAL))) {
    return false;
  }
  return true;
}

void VideoRegionsConfigDialog::updateUI()
{
  StringVector *videoClasses = m_config->getVideoClassNames();
  std::vector<Rect> *videoRects = m_config->getVideoRects();
  StringStorage textAreaData;
  TCHAR endLine[3] = {13, 10, 0};
  {
    AutoLock al(m_config);
    textAreaData.setString(_T(""));
    for (size_t i = 0; i < videoClasses->size(); i++) {
      textAreaData.appendString(videoClasses->at(i).getString());
      textAreaData.appendString(&endLine[0]);
    }
    TCHAR buffer[32];
    _ltot(m_config->getVideoRecognitionInterval(), &buffer[0], 10);
    m_videoRecognitionInterval.setText(buffer);
  }
  m_videoClasses.setText(textAreaData.getString());

  {
    AutoLock al(m_config);
    textAreaData.setString(_T(""));
    for (size_t i = 0; i < videoRects->size(); i++) {
      Rect r = videoRects->at(i);
      StringStorage s;
      RectSerializer::toString(&r, &s);
      textAreaData.appendString(s.getString());
      textAreaData.appendString(&endLine[0]);
    }
  }
  m_videoRects.setText(textAreaData.getString());
}

void VideoRegionsConfigDialog::apply()
{
  // FIXME: Bad code

  
  AutoLock al(m_config);

  //
  // Clear old video classes names container
  //

  StringVector *videoClasses = m_config->getVideoClassNames();
  videoClasses->clear();
  std::vector<Rect> *videoRects = m_config->getVideoRects();
  videoRects->clear();

  //
  // Split text from text area to string array
  //
  
  StringStorage classNames;
  m_videoClasses.getText(&classNames);
  size_t count = 0;
  TCHAR delimiters[] = _T(" \n\r\t,;");

  classNames.split(delimiters, NULL, &count);
  if (count != 0) {
    std::vector<StringStorage> chunks(count);
    classNames.split(delimiters, &chunks.front(), &count);

    for (size_t i = 0; i < count; i++) {
      if (!chunks[i].isEmpty()) {
          videoClasses->push_back(chunks[i].getString());
      }
    }
  }

  StringStorage videoRectsStringStorage;
  m_videoRects.getText(&videoRectsStringStorage);
  count = 0;

  videoRectsStringStorage.split(delimiters, NULL, &count);
  if (count != 0) {
    std::vector<StringStorage> chunks(count);
    videoRectsStringStorage.split(delimiters, &chunks.front(), &count);

    for (size_t i = 0; i < count; i++) {
      if (!chunks[i].isEmpty()) {
        try {
          videoRects->push_back(RectSerializer::toRect(&chunks[i]));
        } catch (...) {
          // Ignore wrong formatted strings
        }
      }
    }
  }

  //
  // TODO: Create parseUInt method
  //

  StringStorage vriss;

  m_videoRecognitionInterval.getText(&vriss);

  int interval;
  StringParser::parseInt(vriss.getString(), &interval);
  m_config->setVideoRecognitionInterval((unsigned int)interval);
}

void VideoRegionsConfigDialog::initControls()
{
  HWND hwnd = m_ctrlThis.getWindow();
  m_videoClasses.setWindow(GetDlgItem(hwnd, IDC_VIDEO_CLASS_NAMES));
  m_videoRects.setWindow(GetDlgItem(hwnd, IDC_VIDEO_RECTS));
  m_videoRecognitionInterval.setWindow(GetDlgItem(hwnd, IDC_VIDEO_RECOGNITION_INTERVAL));
  m_videoRecognitionIntervalSpin.setWindow(GetDlgItem(hwnd, IDC_VIDEO_RECOGNITION_INTERVAL_SPIN));

  int limitersTmp[] = {50, 200};
  int deltasTmp[] = {5, 10};

  std::vector<int> limitters(limitersTmp, limitersTmp + sizeof(limitersTmp) /
                                                        sizeof(int));
  std::vector<int> deltas(deltasTmp, deltasTmp + sizeof(deltasTmp) /
                                                 sizeof(int));

  m_videoRecognitionIntervalSpin.setBuddy(&m_videoRecognitionInterval);
  m_videoRecognitionIntervalSpin.setAccel(0, 1);
  m_videoRecognitionIntervalSpin.setRange32(0, INT_MAX);
  m_videoRecognitionIntervalSpin.setAutoAccelerationParams(&limitters, &deltas, 50);
  m_videoRecognitionIntervalSpin.enableAutoAcceleration(true);
}

void VideoRegionsConfigDialog::onRecognitionIntervalSpinChangePos(LPNMUPDOWN message)
{
  m_videoRecognitionIntervalSpin.autoAccelerationHandler(message);
}

void VideoRegionsConfigDialog::onRecognitionIntervalUpdate()
{
  ((ConfigDialog *)m_parentDialog)->updateApplyButtonState();
}

void VideoRegionsConfigDialog::onVideoRegionsUpdate()
{
  ((ConfigDialog *)m_parentDialog)->updateApplyButtonState();
}
