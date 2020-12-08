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

#ifndef _UI_DATA_ACCESS_H_
#define _UI_DATA_ACCESS_H_

#include "gui/TextBox.h"

/**
 * Gives methods to get typized data from ui controls to
 * avoid code duplicates.
 */
class UIDataAccess
{
public:
  /**
   * Queries text box text value as signed integer.
   * @param textBox [in] text box to get text from.
   * @param value [out, optional] value will contain output result.
   * @return true if text in textbox is valid.
   */
  static bool queryValueAsInt(TextBox *textBox, int *value);
  /**
   * Queries text box text value as unsigned integer.
   * @param textBox [in] text box to get text from.
   * @param value [out, optional] value will contain output result.
   * @return true if text in textbox is valid.
   */
  static bool queryValueAsUInt(TextBox *textBox, unsigned int *value);
private:
  /**
   * Don't allow instanizing of this class, only static methods.
   */
  UIDataAccess();
};

#endif
