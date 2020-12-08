// Copyright (C) 2010,2011,2012 GlavSoft LLC.
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

#include "CommonInputValidation.h"

#include "util/StringParser.h"

#include "gui/BalloonTip.h"

#include "tvnserver/resource.h"

#include "UIDataAccess.h"

CommonInputValidation::CommonInputValidation()
{
}

bool CommonInputValidation::validatePort(TextBox *textBox)
{
  bool validationOk = true;

  int parsedPortValue = 0;

  if (!UIDataAccess::queryValueAsInt(textBox, &parsedPortValue)) {
    validationOk = false;
  } else {
    if (parsedPortValue <= 0 || parsedPortValue > 65535) {
      validationOk = false;
    }
  }

  if (!validationOk) {
    notifyValidationError(textBox, StringTable::getString(IDS_PORT_RANGE_ERROR));
  }

  return validationOk;
}

bool CommonInputValidation::validateUINT(TextBox *textBox, const TCHAR *messageText)
{
  bool validationOk = true;

  unsigned int parsedValue = 0;

  if (!UIDataAccess::queryValueAsUInt(textBox, &parsedValue)) {
    validationOk = false;
  }

  if (!validationOk) {
    notifyValidationError(textBox, messageText);
  }

  return validationOk;
}

void CommonInputValidation::notifyValidationError(TextBox *textBox, const TCHAR *messageText)
{
  BalloonTip invalidInputTip;

  invalidInputTip.setIconType(TTI_WARNING);
  invalidInputTip.setText(messageText);
  invalidInputTip.setTitle(StringTable::getString(IDS_CAPTION_BAD_INPUT));

  textBox->showBalloonTip(&invalidInputTip);
  textBox->setFocus();
}
