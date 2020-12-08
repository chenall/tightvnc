// Copyright (C) 2012 GlavSoft LLC.
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

#include "OptionsDialog.h"

OptionsDialog::OptionsDialog()
: BaseDialog(IDD_OPTIONS),
  m_connected(false)
{
}

void OptionsDialog::setConnectionConfig(ConnectionConfig *conConfig)
{
  m_conConfig = conConfig;
}

void OptionsDialog::setConnected()
{
  m_connected = true;
}

BOOL OptionsDialog::onCommand(UINT controlID, UINT notificationID)
{
  if (controlID == IDOK) {
    if (onOkPressed()) {
      kill(1);
    }
    return TRUE;
  }
  if (controlID == IDCANCEL) {
    kill(0);
    return TRUE;
  }
  if (controlID == IDC_CVIEWONLY) {
    if (notificationID == BN_CLICKED) {
      onViewOnlyClick();
      return TRUE;
    }
  }
  if (controlID == IDC_CEIGHTBIT) {
    if (notificationID == BN_CLICKED) {
      on8BitColorClick();
      return TRUE;
    }
  }
  if (controlID == IDC_CJPEG) {
    if (notificationID == BN_CLICKED) {
      onAllowJpegCompressionClick();
      return TRUE;
    }
  }
  if (controlID == IDC_CCOMPRLVL) {
    if (notificationID == BN_CLICKED) {
      onAllowCustomCompressionClick();
      return TRUE;
    }
  }
  if (controlID == IDC_CUSEENC) {
    if (notificationID == CBN_SELCHANGE) {
      onPreferredEncodingSelectionChange();
      return TRUE;
    }
  }
  if (controlID == IDC_CSCALE) {
    if (notificationID == CBN_KILLFOCUS) {
      onScaleKillFocus();
      return TRUE;
    }
  }
  return FALSE;
}

BOOL OptionsDialog::onInitDialog()
{
  setControlById(m_useEnc, IDC_CUSEENC);
  setControlById(m_eightBit, IDC_CEIGHTBIT);
  setControlById(m_compLvl, IDC_CCOMPRLVL);
  setControlById(m_tcompLvl, IDC_SCOMP);
  setControlById(m_quality, IDC_SQUALITY);
  setControlById(m_jpeg, IDC_CJPEG);
  setControlById(m_tjpeg, IDC_SJPEG);
  setControlById(m_quality2, IDC_SQUALITY2);
  setControlById(m_copyrect, IDC_CCOPYRECT);
  setControlById(m_viewonly, IDC_CVIEWONLY);
  setControlById(m_disclip, IDC_CDISCLIP);
  setControlById(m_sharedses, IDC_CSHAREDSES);
  setControlById(m_scale, IDC_CSCALE);
  setControlById(m_fullscr, IDC_CFULLSCR);
  setControlById(m_deiconfy, IDC_CDEICONFY);
  setControlById(m_swapmouse, IDC_CSWAPMOUSE);
  setControlById(m_track, IDC_RTRACK);
  setControlById(m_cursor, IDC_RCURSOR);
  setControlById(m_ncursor, IDC_RNCURSOR);
  setControlById(m_dot, IDC_RDOT);
  setControlById(m_smalldot, IDC_RSMALLDOT);
  setControlById(m_arrow, IDC_RARROW);
  setControlById(m_nlocal, IDC_RNLOCAL);

  m_useEnc.addItem(_T("Raw"), reinterpret_cast<void *>(EncodingDefs::RAW));
  m_useEnc.addItem(_T("Hextile"), reinterpret_cast<void *>(EncodingDefs::HEXTILE));
  m_useEnc.addItem(_T("Tight"), reinterpret_cast<void *>(EncodingDefs::TIGHT));
  m_useEnc.addItem(_T("RRE"), reinterpret_cast<void *>(EncodingDefs::RRE));
  m_useEnc.addItem(_T("ZRLE"), reinterpret_cast<void *>(EncodingDefs::ZRLE));


  // FIXME: replaced literals to named constants
  TCHAR scaleComboText[8][20] = {_T("25"), _T("50"), _T("75"), _T("90"),
                                 _T("100"), _T("125"), _T("150"), _T("Auto")};
  for (int i = 0; i < sizeof(scaleComboText)/sizeof(scaleComboText[0]); i++) {
      m_scale.addItem(static_cast<TCHAR FAR *>(scaleComboText[i]));
  }

  m_tjpeg.setRange(0, 9);
  m_tcompLvl.setRange(0, 9);
  updateControlValues();
  return FALSE;
}

void OptionsDialog::updateControlValues()
{
  // Preferred encoding
  for (int i = 0; i < m_useEnc.getItemsCount(); i++) {
    int enc = reinterpret_cast<int>(m_useEnc.getItemData(i));
    if (enc == m_conConfig->getPreferredEncoding()) {
      m_useEnc.setSelectedItem(i);
      break;
    } // if found

    // set default value, if preferred encoding not in list
    if (enc == EncodingDefs::HEXTILE)
      m_useEnc.setSelectedItem(i);
  } // for i

  m_eightBit.check(m_conConfig->isUsing8BitColor());

  m_compLvl.check(m_conConfig->isCustomCompressionEnabled());
  m_jpeg.check(m_conConfig->isJpegCompressionEnabled());

  m_copyrect.check(m_conConfig->isCopyRectAllowed());
  m_viewonly.check(m_conConfig->isViewOnly());
  m_disclip.check(!m_conConfig->isClipboardEnabled());
  m_fullscr.check(m_conConfig->isFullscreenEnabled());
  m_deiconfy.check(m_conConfig->isDeiconifyOnRemoteBellEnabled());
  m_swapmouse.check(m_conConfig->isMouseSwapEnabled());

  m_sharedses.check(m_conConfig->getSharedFlag());
  m_sharedses.setEnabled(!m_connected);

  if (m_conConfig->isFitWindowEnabled()) {
    // FIXME: replace literal to named constant
    m_scale.setSelectedItem(7);
  } else {
    int n = m_conConfig->getScaleNumerator();
    int d = m_conConfig->getScaleDenominator();

    int percent = (n * 100) / d;

    StringStorage text;
    text.format(_T("%d"), percent);

    m_scale.setText(text.getString());
  }

  {
    bool enableCursorUpdate = m_conConfig->isRequestingShapeUpdates();
    bool ignoreCursorUpdate = m_conConfig->isIgnoringShapeUpdates();
    m_track.check(enableCursorUpdate && !ignoreCursorUpdate);
    m_cursor.check(!enableCursorUpdate && ignoreCursorUpdate);
    m_ncursor.check(enableCursorUpdate && ignoreCursorUpdate);
  }

  StringStorage labelText;
  {
    const int DEFAULT_COMPRESSION_LEVEL = 6;
    int level = DEFAULT_COMPRESSION_LEVEL;
    if (m_conConfig->isCustomCompressionEnabled())
      level = m_conConfig->getCustomCompressionLevel();
    m_tcompLvl.setPos(level);
    labelText.format(_T("%d"), level);
    m_quality.setText(labelText.getString());
  }

  {
  const int DEFAULT_JPEG_COMPRESSION_LEVEL = 6;
    int level = DEFAULT_JPEG_COMPRESSION_LEVEL;
    if (m_conConfig->isJpegCompressionEnabled())
      level = m_conConfig->getJpegCompressionLevel();
    m_tjpeg.setPos(level);
    labelText.format(_T("%d"), level);
    m_quality2.setText(labelText.getString());
  }

  switch (m_conConfig->getLocalCursorShape()) {
  case ConnectionConfig::SMALL_CURSOR:
    m_smalldot.check(true);
    break;
  case ConnectionConfig::NORMAL_CURSOR:
    m_arrow.check(true);
    break;
  case ConnectionConfig::NO_CURSOR:
    m_nlocal.check(true);
    break;
  default:
    m_dot.check(true);
    break;
  }

  onViewOnlyClick();

  onAllowCustomCompressionClick();
  onAllowJpegCompressionClick();

  onCustomCompressionLevelScroll();
  onJpegCompressionLevelScroll();

  onPreferredEncodingSelectionChange();
  on8BitColorClick();
}

void OptionsDialog::onViewOnlyClick()
{
  if (m_viewonly.isChecked()) {
    m_swapmouse.setEnabled(false);
  } else {
    m_swapmouse.setEnabled(true);
  }
}

void OptionsDialog::on8BitColorClick()
{
  if (!m_eightBit.isChecked()) {
    if (m_jpeg.isChecked()) {
      enableJpegCompression(true);
    }
    m_jpeg.setEnabled(true);
  } else {
    m_jpeg.setEnabled(false);
    enableJpegCompression(false);
  }
}

void OptionsDialog::enableJpegCompression(bool enable)
{
  m_tjpeg.setEnabled(enable);
  m_quality2.setEnabled(enable);
  EnableWindow(GetDlgItem(getControl()->getWindow(), IDC_SPOOR), enable);
  EnableWindow(GetDlgItem(getControl()->getWindow(), IDC_SBEST2), enable);
  EnableWindow(GetDlgItem(getControl()->getWindow(), IDC_STQUALITY2), enable);
}
void OptionsDialog::onAllowCustomCompressionClick()
{
  enableCustomCompression(m_compLvl.isChecked());
}

void OptionsDialog::enableCustomCompression(bool enable)
{
  m_tcompLvl.setEnabled(enable);
  m_quality.setEnabled(enable);
  EnableWindow(GetDlgItem(getControl()->getWindow(), IDC_SBEST), enable);
  EnableWindow(GetDlgItem(getControl()->getWindow(), IDC_SFAST), enable);
  EnableWindow(GetDlgItem(getControl()->getWindow(), IDC_STQUALITY), enable);
}

void OptionsDialog::onAllowJpegCompressionClick()
{
  enableJpegCompression(m_jpeg.isChecked());
}

void OptionsDialog::onPreferredEncodingSelectionChange()
{
  int index = m_useEnc.getSelectedItemIndex();
  if (index < 0) {
    return ;
  }
  int encoding = reinterpret_cast<int>(m_useEnc.getItemData(index));
  switch (encoding) {
  case EncodingDefs::TIGHT:
    enableCustomCompression(m_compLvl.isChecked());
    m_compLvl.setEnabled(true);
    break;
  default:
    enableCustomCompression(false);
    m_compLvl.setEnabled(false);
    break;
  } // switch
} // void

void OptionsDialog::onCustomCompressionLevelScroll()
{
  StringStorage labelText;
  labelText.format(_T("%ld"), m_tcompLvl.getPos());
  m_quality.setText(labelText.getString());
}

void OptionsDialog::onJpegCompressionLevelScroll()
{
  StringStorage labelText;
  labelText.format(_T("%ld"), m_tjpeg.getPos());
  m_quality2.setText(labelText.getString());
}

void OptionsDialog::onMessageReceived(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  switch (uMsg) {
  case WM_HSCROLL:
    if (HWND(lParam) == m_tcompLvl.getWindow()) {
      onCustomCompressionLevelScroll();
    }
    if (HWND(lParam) == m_tjpeg.getWindow()) {
      onJpegCompressionLevelScroll();
    }
    break;
  }
}

void OptionsDialog::onScaleKillFocus()
{
  StringStorage scaleText;
  m_scale.getText(&scaleText);

  int scale;

  if (!StringParser::parseInt(scaleText.getString(), &scale)) {
    if (scaleText.isEqualTo(_T("Auto"))) {
      return ;
    }
    scale = 100;
  }

  if (scale < 1) {
    scale = 1;
  } else if (scale > 400) {
    scale = 400;
  }

  scaleText.format(_T("%d"), scale);
  m_scale.setText(scaleText.getString());
}

bool OptionsDialog::isInputValid()
{
  int scaleInt;
  StringStorage scaleText;

  m_scale.getText(&scaleText);

  if (scaleText.isEqualTo(_T("Auto"))) {
    return true;
  }

  if (!StringParser::parseInt(scaleText.getString(), &scaleInt)) {
    StringStorage error;
    error.format(StringTable::getString(IDS_ERROR_VALUE_FIELD_ONLY_NUMERIC),
                 StringTable::getString(IDS_OPTIONS_SCALE));
    MessageBox(m_ctrlThis.getWindow(),
               error.getString(),
               StringTable::getString(IDS_OPTIONS_CAPTION),
               MB_OK | MB_ICONWARNING);
    return false;
  }

  if (scaleInt < 0) {
    StringStorage error;
    error.format(StringTable::getString(IDS_ERROR_VALUE_FIELD_ONLY_POSITIVE_NUMERIC),
                 StringTable::getString(IDS_OPTIONS_SCALE));
    MessageBox(m_ctrlThis.getWindow(),
               error.getString(),
               StringTable::getString(IDS_OPTIONS_CAPTION),
               MB_OK | MB_ICONWARNING);
    return false;
  }

  return true;
}

bool OptionsDialog::onOkPressed()
{
  if (!isInputValid())
    return false;
  apply();
  return true;
}

void OptionsDialog::apply()
{
  // Preferred encoding
  int pesii = m_useEnc.getSelectedItemIndex();
  if (pesii >= 0) {
    int preferredEncoding = reinterpret_cast<int>(m_useEnc.getItemData(pesii));
    m_conConfig->setPreferredEncoding(preferredEncoding);
  } else {
    _ASSERT(pesii >= 0);
    m_conConfig->setPreferredEncoding(EncodingDefs::TIGHT);
  }

  if (m_compLvl.isChecked()) {
    int level = static_cast<int>(m_tcompLvl.getPos());
    m_conConfig->setCustomCompressionLevel(level);
  } else {
    m_conConfig->disableCustomCompression();
  }

  if (m_jpeg.isChecked()) {
    int level = static_cast<int>(m_tjpeg.getPos());
    m_conConfig->setJpegCompressionLevel(level);
  } else {
    m_conConfig->disableJpegCompression();
  }

  m_conConfig->use8BitColor(m_eightBit.isChecked());
  m_conConfig->allowCopyRect(m_copyrect.isChecked());
  m_conConfig->setViewOnly(m_viewonly.isChecked());
  m_conConfig->enableClipboard(!m_disclip.isChecked());
  m_conConfig->enableFullscreen(m_fullscr.isChecked());
  m_conConfig->deiconifyOnRemoteBell(m_deiconfy.isChecked());
  m_conConfig->swapMouse(m_swapmouse.isChecked());
  m_conConfig->setSharedFlag(m_sharedses.isChecked());

  StringStorage scaleText;

  m_scale.getText(&scaleText);

  int scaleInt = 0;

  if (StringParser::parseInt(scaleText.getString(), &scaleInt)) {
    m_conConfig->setScale(scaleInt, 100);
    m_conConfig->fitWindow(false);
  } else {
    m_conConfig->fitWindow(true);
  }

  if (m_track.isChecked()) {
    m_conConfig->requestShapeUpdates(true);
    m_conConfig->ignoreShapeUpdates(false);
  }

  if (m_cursor.isChecked()) {
    m_conConfig->requestShapeUpdates(false);
    m_conConfig->ignoreShapeUpdates(true);
  }

  if (m_ncursor.isChecked()) {
    m_conConfig->requestShapeUpdates(true);
    m_conConfig->ignoreShapeUpdates(true);
  }

  int localCursorShape = ConnectionConfig::DOT_CURSOR;
  if (m_smalldot.isChecked()) {
    localCursorShape = ConnectionConfig::SMALL_CURSOR;
  } else if (m_arrow.isChecked()) {
    localCursorShape = ConnectionConfig::NORMAL_CURSOR;
  } else if (m_nlocal.isChecked()) {
    localCursorShape = ConnectionConfig::NO_CURSOR;
  }

  m_conConfig->setLocalCursorShape(localCursorShape);
}
