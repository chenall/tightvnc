// Copyright (C) 2008, 2009, 2010 GlavSoft LLC.
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

#pragma once

#include "rfb/RfbKeySym.h"
#include <vector>

class KeySymTest : public RfbKeySymListener
{
public:
  KeySymTest(const TCHAR *fileFrom, const TCHAR *fileTo);
  virtual ~KeySymTest();

  int run();

private:
  virtual void onRfbKeySymEvent(unsigned int rfbKeySym, bool down);

  static const TCHAR VALID_WORD_LETTERS[];

  // Removes comment from the line. Comments must start with the "#" symbol.
  void removeComments(StringStorage *line, StringStorage *extractedComment);

  bool readLine(StringStorage *line);

  // pos in - position to start search a word, out - position to start search
  // for next word.
  // Returns true if word has been found.
  bool getWord(const StringStorage *line, size_t *pos, StringStorage *word);

  void changeKbdLayout(HKL hkl);

  RfbKeySym *m_rfbKeySym;
  StringStorage m_fromFileName;
  StringStorage m_toFileName;
  FILE *m_fFrom;
  FILE *m_fTo;
  size_t m_lineNumber;

  bool m_isNextEventInSeries;

  LogWriter m_log;
};
