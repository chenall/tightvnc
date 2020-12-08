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

#include "KeySymTest.h"
#include "util/Exception.h"
#include "util/StringParser.h"

const TCHAR KeySymTest::VALID_WORD_LETTERS[] = _T("zyxwvutsrqponmlkjihgfedcba")
                                               _T("ZYXWVUTSRQPONMLKJIHGFEDCBA")
                                               _T("01234567890");

KeySymTest::KeySymTest(const TCHAR *fileFrom, const TCHAR *fileTo)
: m_fTo(0),
  m_fFrom(0),
  m_lineNumber(0),
  m_fromFileName(fileFrom),
  m_toFileName(fileTo),
  m_isNextEventInSeries(false),
  m_log(0)
{
  m_rfbKeySym = new RfbKeySym(this, &m_log);
  m_fFrom = _tfopen(m_fromFileName.getString(), _T("rt,ccs=UNICODE"));
  if (m_fFrom == 0) {
    StringStorage errMess;
    errMess.format(_T("Cannot open the %s file"), m_fromFileName.getString());
    throw Exception(errMess.getString());
  }
  m_fTo = _tfopen(m_toFileName.getString(), _T("wt,ccs=UNICODE"));
  if (m_fTo == 0) {
    StringStorage errMess;
    errMess.format(_T("Cannot open the %s file"), m_toFileName.getString());
    throw Exception(errMess.getString());
  }
}

KeySymTest::~KeySymTest()
{
  fclose(m_fFrom);
  fclose(m_fTo);
  delete m_rfbKeySym;
}

int KeySymTest::run()
{
  StringStorage line;
  while (readLine(&line)) {
    m_lineNumber++;
    size_t linePos = 0;
    StringStorage comment;
    removeComments(&line, &comment);
    // Split to words
    StringStorage word1, word2;
    if (getWord(&line, &linePos, &word1) &&
        getWord(&line, &linePos, &word2)) {
      if (word1.isEqualTo(_T("kbdlayout"))) {
        // Try parse word2 as a hexadecimal value
        unsigned int hkbdLayout = 0;
        if (!StringParser::parseHex(word2.getString(), &hkbdLayout)) {
          StringStorage errMess;
          errMess.format(_T("Wrong \"kbdlayout\" argument at %u line (%s)"),
                         m_lineNumber,
                         m_fromFileName.getString());
          throw Exception(errMess.getString());
        }
        changeKbdLayout((HKL)hkbdLayout);
        Sleep(500);
      } else {
        unsigned int virtKeyInt, downInt;
        bool validWord = StringParser::parseUInt(word1.getString(), &virtKeyInt);
        validWord = validWord &&
                    StringParser::parseUInt(word2.getString(), &downInt);
        if (validWord && (downInt == 0 || downInt == 1)) {
          unsigned char virtKey = virtKeyInt & 255;
          bool down = downInt != 0;
          unsigned int addKeyData = 0;
          addKeyData = down ? 0 : 0x80000000;
          m_rfbKeySym->processKeyEvent(virtKey, addKeyData);
        } else {
          StringStorage errMess;
          errMess.format(_T("Wrong value(s) at %u line (%s)"),
                         m_lineNumber,
                         m_fromFileName.getString());
          throw Exception(errMess.getString());
        }
      }
    }
    _ftprintf(m_fTo, _T("%s\n"), comment.getString());
    m_isNextEventInSeries = false;
  }
  return 0;
}

void KeySymTest::changeKbdLayout(HKL hkl)
{
  if (ActivateKeyboardLayout(hkl, 0) == 0) {
    StringStorage errMess;
    errMess.format(_T("Can't apply a keyboard layout requested at the %u line (%s)"),
                   m_lineNumber,
                   m_fromFileName.getString());
    throw Exception(errMess.getString());
  }
}

bool KeySymTest::readLine(StringStorage *line)
{
  TCHAR buff[255];
  if (_fgetts(buff, sizeof(buff) / sizeof(TCHAR), m_fFrom) != 0) {
    line->setString(buff);
    return true;
  }
  return false;
}

void KeySymTest::removeComments(StringStorage *line,
                                StringStorage *extractedComment)
{
  extractedComment->setString(_T(""));
  // Find the "#" symbol
  size_t commentStartPos = _tcscspn(line->getString(), _T("#"));
  if (commentStartPos >= line->getLength()) {
    return; // No comments found
  }
  extractedComment->setString(line->getString() + commentStartPos);
  // Reject the '\n' symbols from extractedComment
  if (extractedComment->endsWith('\n')) {
    extractedComment->truncate(1);
  }
  line->truncate(line->getLength() - commentStartPos);
}

bool KeySymTest::getWord(const StringStorage *line,
                         size_t *pos,
                         StringStorage *word)
{
  if (*pos >= line->getLength()) {
    return false;
  }
  size_t wordStartPos = _tcscspn(line->getString() + *pos, VALID_WORD_LETTERS);
  wordStartPos += *pos;
  size_t wordEndPos = _tcsspn(line->getString() + wordStartPos,
                              VALID_WORD_LETTERS);
  wordEndPos += wordStartPos;
  // Truncate the string
  line->getSubstring(word, wordStartPos, wordEndPos - 1);

  *pos = wordEndPos;
  return true;
}

void KeySymTest::onRfbKeySymEvent(unsigned int rfbKeySym, bool down)
{
  // Separating next event from previous by new line.
  if (m_isNextEventInSeries) {
    _ftprintf(m_fTo, _T("\n"));
  }
  _ftprintf(m_fTo, _T("%d %#4.4x           "), int(down), (unsigned int)rfbKeySym);
  m_isNextEventInSeries = true;
}
