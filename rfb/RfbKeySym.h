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

#ifndef __RFBKEYSYM_H__
#define __RFBKEYSYM_H__

#include "RfbKeySymListener.h"
#include "util/Keymap.h"
#include "log-writer/LogWriter.h"

// Translates pressed key to a series of rfb symbols. Gives the series
// to out by the listener function serial calling.
class RfbKeySym
{
public:
  RfbKeySym(RfbKeySymListener *extKeySymListener, LogWriter *log);
  virtual ~RfbKeySym();

  // This function doesn't distinguish between left and right modifiers.
  void sendModifier(unsigned char virtKey, bool down);

  void processKeyEvent(unsigned short virtKey, unsigned int addKeyData);
  bool vkCodeToString(unsigned short virtKey, bool isKeyDown, std::wstring *res);
  void processCharEvent(WCHAR charCode, unsigned int addKeyData);
  // Checks a new modifiers state after focus restoration and sends difference
  void processFocusRestoration();
  // This function release all modifiers unconditionally.
  void processFocusLoss();

  void sendCtrlAltDel();

  // Set function for m_winKeyIgnore.
  void setWinKeyIgnore(bool winIgnore);

private:
  void clearKeyState();

  void releaseMeta();
  void restoreMeta();
  void releaseModifier(unsigned char modifier);
  void restoreModifier(unsigned char modifier);
  void releaseModifiers();
  void restoreModifiers();
  
  bool isPressed(unsigned char virtKey);

  // This function does distinguish between a right or left modifier and
  // if the virtKey is a modifier (e.g VK_CONTROL), the function always return
  // a left- or right-hand virtual key value (e.g. VK_LCONTROL or VK_RCONTROL)
  // by taking into account the isRightHint flag.
  // If virtKey is not a modifier, the function returns virtKey value without
  // changing.
  unsigned char distinguishLeftRightModifier(unsigned char virtKey,
                                             bool isRightHint);

  // Checks virtKey state with the server side state and sends difference
  void checkAndSendDiff(unsigned char virtKey, unsigned char state);

  // Send one key event (Alt translated to Meta if Scroll Lock is on).
  virtual void sendKeySymEvent(unsigned int rfbKeySym, bool down);

  // Send one key event (Alt not translated to Meta).
  virtual void sendVerbatimKeySymEvent(unsigned int rfbKeySym, bool down);

  // helper functions
  WCHAR GettingCharFromCtrlSymbol(WCHAR ctrlSymbol);
  // E.g if pressed Ctrl + Alt + A
  // Try found char without modificators
  bool TryTranslateNotPrintableToUnicode(unsigned short virtKey, HKL currentLayout, WCHAR *unicodeChar);

  RfbKeySymListener *m_extKeySymListener;

  // This state doesn't difference between left and right modifiers. It's
  // needed to ToUnicodeEx().
  unsigned char m_viewerKeyState[256];

  // This state does difference between left and right modifiers. It's
  // needed to know the server side state (e.g. to release or restore
  // modifiyers state outside from a real key event).
  unsigned char m_serverKeyState[256];
  bool m_leftMetaIsPressed;
  bool m_rightMetaIsPressed;

  Keymap m_keyMap;
  bool m_allowProcessCharEvent;
  bool m_allowProcessDoubleChar;
  bool m_doubleDeadCatched;
  
  LogWriter *m_log;

  // Flag for ignoring win key.
  bool m_winKeyIgnore;
};

#endif // __RFBKEYSYM_H__
