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

#ifndef _COMMON_INPUT_VALIDATION_H_
#define _COMMON_INPUT_VALIDATION_H_

#include "gui/TextBox.h"

/**
 * Gives static methods set to validate integer data in text boxes.
 */
class CommonInputValidation
{
public:
  /**
   * Validates port value in [1..65535] range.
   * @param textBox text box text of that need to validate.
   * @return true if validation ok, false if validation failed.
   * @note that if validation fails, then it shows error notification
   * using notifyValidationError() method with text from resources.
   * @note that message text id defined in method code.
   */
  static bool validatePort(TextBox *textBox);
  /**
   * Validates unsigned integer value.
   * @param textBox text box text of that need to validate.
   * @param messageText text to show on validation fail.
   * @return true if validation ok, false if validation failed.
   * @note that if validation fails, then it shows error notification
   * using notifyValidationError() method.
   */
  static bool validateUINT(TextBox *textBox, const TCHAR *messageText);
  /**
   * Shows validation error message (message box or balloon tip, depends on realization)
   * and sets focus to control.
   * @param textBox text box in that validation failed.
   * @param messageText message describing error reason.
   * @remark current realization supports only message boxes through
   * BaloonTip interface.
   * @fixme don't use BaloonTip in this case, use direct message box.
   */
  static void notifyValidationError(TextBox *textBox, const TCHAR *messageText);
private:
  /**
   * Don't allow creating instances of this class.
   */
  CommonInputValidation();
};

#endif
