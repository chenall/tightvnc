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

#include "RfbKeySym.h"
#include "util/CommonHeader.h"

#define XK_MISCELLANY
#include "rfb/keysymdef.h"

RfbKeySym::RfbKeySym(RfbKeySymListener *extKeySymListener, LogWriter *log)
: m_extKeySymListener(extKeySymListener),
  m_allowProcessCharEvent(false),
  m_allowProcessDoubleChar(false),
  m_doubleDeadCatched(false),
  m_leftMetaIsPressed(false),
  m_rightMetaIsPressed(false),
  m_log(log),
  m_winKeyIgnore(true)
{
  clearKeyState();
}

RfbKeySym::~RfbKeySym()
{
}

void RfbKeySym::sendModifier(unsigned char virtKey, bool down)
{
  UINT32 rfbSym;
  bool success = m_keyMap.virtualCodeToKeySym(&rfbSym, virtKey);
  _ASSERT(success);
  sendKeySymEvent(rfbSym, down);

  m_viewerKeyState[virtKey] = down ? 128 : 0;
  virtKey = distinguishLeftRightModifier(virtKey, false);
  m_serverKeyState[virtKey] = down ? 128 : 0;
}

void RfbKeySym::processKeyEvent(unsigned short virtKey,
                                unsigned int addKeyData)
{
  m_log->debug(_T("processKeyEvent() function called: virtKey = %#4.4x, addKeyData")
             _T(" = %#x"), (unsigned int)virtKey, addKeyData);
  // Ignoring win key (when fullscreen mode is off).
  if (m_winKeyIgnore) {
    if (virtKey == VK_LWIN || virtKey == VK_RWIN) { // Ignoring the Win key
      m_log->debug(_T("Ignoring the Win key event"));
      return;
    }
  }

  bool down = (addKeyData & 0x80000000) == 0;
  m_log->debug(_T("down = %u"), (unsigned int)down);

  bool ctrlPressed = isPressed(VK_LCONTROL) || isPressed(VK_RCONTROL);
  bool altPressed = isPressed(VK_LMENU) || isPressed(VK_RMENU);
  bool shiftPressed = isPressed(VK_LSHIFT) || isPressed(VK_RSHIFT);
  bool capsToggled = (GetKeyState(VK_CAPITAL) & 1) != 0;
  m_log->debug(_T("ctrl = %u, alt = %u, shift = %u, caps toggled = %u"),
    (unsigned int)ctrlPressed,
    (unsigned int)altPressed,
    (unsigned int)shiftPressed,
    (unsigned int)capsToggled);

  // Without distinguishing between left and right modifiers.
  m_viewerKeyState[virtKey & 255] = down ? 128 : 0;
  m_viewerKeyState[VK_CAPITAL & 255] = capsToggled ? 1 : 0;

  bool extended = (addKeyData & 0x1000000) != 0; // 24 bit
  virtKey = distinguishLeftRightModifier((unsigned char)virtKey, extended);

  // With distinguishing between left and right modifiers.
  m_serverKeyState[virtKey & 255] = down ? 128 : 0;
  UINT32 rfbSym;
  if (m_keyMap.virtualCodeToKeySym(&rfbSym, virtKey & 255)) {
    // Special case for VK_RETURN that have no self numpad code.
    // FIXME: May be replace this code to the virtualCodeToKeySym() function?
    if (rfbSym == XK_Return && extended) {
      rfbSym = XK_KP_Enter;
    }

    m_log->debug(_T("The key has been mapped to the %#4.4x rfb symbol"),
      rfbSym);
    sendKeySymEvent(rfbSym, down);
  } else {
    std::wstring chars;
    bool needReleaseModifiers = vkCodeToString(virtKey, down, &chars);
    if (ctrlPressed && altPressed && needReleaseModifiers) {
      m_log->debug(_T("Release the ctrl and alt")
        _T(" modifiers before send the key event(s)"));
      releaseModifiers();
    }

    for (int i = 0; i < chars.size(); i++) {
      if (m_keyMap.unicodeCharToKeySym(chars[i], &rfbSym)) {
        sendKeySymEvent(rfbSym, down);
      } else {
        m_log->error(_T("Can't translate the %#4.4x unicode character to an")
          _T(" rfb symbol to send it"), (unsigned int)chars[i]);
      }
    }
    if (ctrlPressed && altPressed && needReleaseModifiers) {
      m_log->debug(_T("Restore the ctrl and alt")
        _T(" modifiers after send the key event(s)"));
      restoreModifiers();
    }
  }
}

bool isDeadCharacter(WCHAR ch)
{
  return (ch == '^'    || 
          ch == 0x00a8 || // 
          ch == '~'    || 
          ch == 0x00b4 || // 
          ch == '`'    ||
          ch == 0x02c7 || // 
          ch == 0x02d8 || // 
          ch == 0x00b0 || // 
          ch == 0x02d9 || // 
          ch == 0x02dd || //
          ch == 0x00b8 || // 
          ch == 0x02db    //
    );
}

bool isDoubleDeadCharacters(WCHAR *buff)
{
  return (isDeadCharacter(buff[0]) && buff[0] == buff[1]);
}

WCHAR RfbKeySym::GettingCharFromCtrlSymbol(WCHAR ch)
{
  bool ctrlPressed = isPressed(VK_LCONTROL) || isPressed(VK_RCONTROL);
  bool altPressed = isPressed(VK_LMENU) || isPressed(VK_RMENU);
  bool shiftPressed = isPressed(VK_LSHIFT) || isPressed(VK_RSHIFT);
  unsigned int oldCh = (unsigned int)ch;
  if (ctrlPressed && !altPressed && ch < 32) {
    if (ch >= 1 && ch <= 26 && !shiftPressed) {
      ch += 96;
    }
    else {
      ch += 64;
    }
    m_log->debug(_T("The %u char is a control symbol then")
      _T(" it will be increased to %u"),
      oldCh, (unsigned int)ch);
  }
  return ch;
}

bool RfbKeySym::TryTranslateNotPrintableToUnicode(unsigned short virtKey, HKL currentLayout, WCHAR *unicodeChar)
{
  WCHAR outBuff[20];
  m_log->debug(_T("Was get a not printable symbol then try get a printable")
    _T(" with turned off the ctrl and alt modifiers"));
  // E.g if pressed Ctrl + Alt + A
  // Try found char without modificators
  unsigned char withoutCtrlAltKbdState[256];
  memcpy(withoutCtrlAltKbdState, m_viewerKeyState, sizeof(withoutCtrlAltKbdState));
  withoutCtrlAltKbdState[VK_LCONTROL] = 0;
  withoutCtrlAltKbdState[VK_RCONTROL] = 0;
  withoutCtrlAltKbdState[VK_CONTROL] = 0;
  withoutCtrlAltKbdState[VK_LMENU] = 0;
  withoutCtrlAltKbdState[VK_RMENU] = 0;
  withoutCtrlAltKbdState[VK_MENU] = 0;
  int count = ToUnicodeEx(virtKey, 0, withoutCtrlAltKbdState, outBuff,
    sizeof(outBuff) / sizeof(WCHAR),
    0, currentLayout);
  if (count > 0) {
    outBuff[count] = 0;
  }
  else {
    outBuff[0] = 0;
  }
  m_log->debug(_T("ToUnicodeEx() without ctrl and alt return %d wchars : %S"), count, outBuff);

  if (count == 1) { // other case will be ignored
    *unicodeChar = outBuff[0];
    return true;
  }
  return false;
}

bool RfbKeySym::vkCodeToString(unsigned short virtKey, bool down, std::wstring *res) 
{
  bool needReleaseModifiers = false;
  WCHAR outBuff[20];

  HKL currentLayout = GetKeyboardLayout(0);

  int count = ToUnicodeEx(virtKey, 0, m_viewerKeyState, outBuff,
      sizeof(outBuff) / sizeof(WCHAR),
      0, currentLayout);

  // For keyboards with dead keys
  if (m_doubleDeadCatched && !down && !m_allowProcessDoubleChar) {
    m_log->debug(_T("Disable process char event"));
    m_allowProcessCharEvent = false;
    m_doubleDeadCatched = false;
  }

  // For keyboards with dead keys
  if (count == 2 && (isDoubleDeadCharacters(outBuff))) {
    m_log->debug(_T("Extra double dead key catched."));
    if (down) {
      m_log->debug(_T("Enable process char event. Enable process double char."));
      m_allowProcessDoubleChar = true;
      m_allowProcessCharEvent = true;
      m_doubleDeadCatched = true;
      count = ToUnicodeEx(virtKey, 0, m_viewerKeyState, outBuff,
          sizeof(outBuff) / sizeof(WCHAR),
          0, currentLayout);
      if (count > 0) {
        outBuff[count] = 0;
      } else {
        outBuff[0] = 0;
      }
      //ToUnicodeEx can return without null termination
      m_log->debug(_T("ToUnicodeEx() function called second. Returned: %d. Output buff: %S"), count, outBuff);
      res->erase();
      return false;
    } else if (!m_allowProcessCharEvent) {
      m_log->debug(_T("Disable second calling ToUnicodeEx()"));
      res->erase();
      return false;
    }
  }

  if (count > 0) {
    count = ToUnicodeEx(virtKey, 0, m_viewerKeyState, outBuff,
          sizeof(outBuff) / sizeof(WCHAR), 0, currentLayout);
  }
  if (count > 0) {
    outBuff[count] = 0;
  } else {
    outBuff[0] = 0;
  }
  m_log->debug(_T("ToUnicodeEx() returned %d, output buff : %S"), count, outBuff);

  if (count == 1 && !m_allowProcessCharEvent) {
    res->push_back(GettingCharFromCtrlSymbol(outBuff[0]));
    needReleaseModifiers = true;
  }
  // Two or more (only two of them is relevant?)
  else if (count > 1) {
    for (int i = 0; i < count; i++) {
      res->push_back(GettingCharFromCtrlSymbol(outBuff[i]));
    }
    needReleaseModifiers = true;
  } else if (count == 0) {
    WCHAR unicodeChar;
    if (TryTranslateNotPrintableToUnicode(virtKey, currentLayout, &unicodeChar)) { 
      res->push_back(unicodeChar);
    }
  } else if (count == -1 && down) {
      m_log->debug(_T("Dead key pressed, wait for a char event"));
      m_allowProcessCharEvent = true;
  }
  return needReleaseModifiers;
}

void RfbKeySym::processCharEvent(WCHAR charCode,
                                 unsigned int addKeyData)
{
  if (m_allowProcessCharEvent) {
    m_log->debug(_T("processCharEvent() function called with alowed processing:")
               _T(" charCode = %#4.4x, addKeyData = %#x"),
               (unsigned int)charCode, addKeyData);
    // For keyboards with dead keys
    if (m_allowProcessDoubleChar) {
      m_allowProcessDoubleChar = false;
      bool ctrlPressed = isPressed(VK_LCONTROL) || isPressed(VK_RCONTROL);
      bool altPressed = isPressed(VK_LMENU) || isPressed(VK_RMENU);
      if (ctrlPressed && altPressed) {
        m_log->debug(_T("Release the ctrl and alt")
          _T(" modifiers before send dead combination"));
        releaseModifiers();
      }
    } else {
      m_allowProcessCharEvent = false;
    }

    UINT32 rfbSym;
    if (m_keyMap.unicodeCharToKeySym(charCode, &rfbSym)) {
      sendKeySymEvent(rfbSym, true);
      sendKeySymEvent(rfbSym, false);
    } else {
      m_log->error(_T("Can't translate the %#4.4x unicode character to an")
                 _T(" rfb symbol to send it"), (unsigned int)charCode);
    }
  }
}

void RfbKeySym::processFocusRestoration()
{
  m_log->info(_T("Process focus restoration in the RfbKeySym class"));

  // Send a modifier key state based on physical keyboard state, not thread key message queue.
  unsigned char keys[9] = {VK_CONTROL, VK_RCONTROL, VK_LCONTROL,
                           VK_MENU, VK_RMENU, VK_LMENU, 
                           VK_SHIFT, VK_RSHIFT, VK_LSHIFT};
  for (int i = 0; i < 9 ; i++) {
    unsigned char key = keys[i];
    unsigned char state = GetAsyncKeyState(key) >> 8;
    checkAndSendDiff(key, state);
  }
}

void RfbKeySym::processFocusLoss()
{
  m_log->info(_T("Process focus loss in the RfbKeySym class"));

  // Send a modifier key up only if the key is down.
  checkAndSendDiff(VK_CONTROL, 0);
  checkAndSendDiff(VK_RCONTROL, 0);
  checkAndSendDiff(VK_LCONTROL, 0);
  checkAndSendDiff(VK_MENU, 0);
  checkAndSendDiff(VK_LMENU, 0);
  checkAndSendDiff(VK_RMENU, 0);
  checkAndSendDiff(VK_SHIFT, 0);
  checkAndSendDiff(VK_RSHIFT, 0);
  checkAndSendDiff(VK_LSHIFT, 0);
  checkAndSendDiff(VK_LWIN, 0);
  checkAndSendDiff(VK_RWIN, 0);
}

void RfbKeySym::sendCtrlAltDel()
{
  releaseModifier(VK_RWIN);
  releaseModifier(VK_LWIN);
  releaseModifier(VK_LSHIFT);
  releaseModifier(VK_RSHIFT);
  releaseMeta();

  sendVerbatimKeySymEvent(XK_Control_L, true);
  sendVerbatimKeySymEvent(XK_Alt_L, true);
  sendVerbatimKeySymEvent(XK_Delete, true);
  sendVerbatimKeySymEvent(XK_Delete, false);
  sendVerbatimKeySymEvent(XK_Alt_L, false);
  sendVerbatimKeySymEvent(XK_Control_L, false);

  restoreMeta();
  restoreModifier(VK_RSHIFT);
  restoreModifier(VK_LSHIFT);
  restoreModifier(VK_LWIN);
  restoreModifier(VK_RWIN);
}

void RfbKeySym::setWinKeyIgnore(bool winKeyIgnore)
{
  m_winKeyIgnore = winKeyIgnore;
}

void RfbKeySym::releaseModifiers()
{
  releaseModifier(VK_LCONTROL);
  releaseModifier(VK_RCONTROL);
  releaseModifier(VK_LMENU);
  releaseModifier(VK_RMENU);
}

void RfbKeySym::restoreModifiers()
{
  restoreModifier(VK_LCONTROL);
  restoreModifier(VK_RCONTROL);
  restoreModifier(VK_LMENU);
  restoreModifier(VK_RMENU);
}

void RfbKeySym::releaseModifier(unsigned char modifier)
{
  UINT32 rfbSym;
  if (isPressed(modifier)) {
    bool success = m_keyMap.virtualCodeToKeySym(&rfbSym, modifier);
    _ASSERT(success);
    sendKeySymEvent(rfbSym, false);
  }
}

void RfbKeySym::releaseMeta()
{
  if (m_leftMetaIsPressed) {
    sendKeySymEvent(XK_Meta_L, false);
  }
  if (m_rightMetaIsPressed) {
    sendKeySymEvent(XK_Meta_R, false);
  }
}

void RfbKeySym::restoreMeta()
{
  if (m_leftMetaIsPressed) {
    sendKeySymEvent(XK_Meta_L, true);
  }
  if (m_rightMetaIsPressed) {
    sendKeySymEvent(XK_Meta_R, true);
  }
}

void RfbKeySym::restoreModifier(unsigned char modifier)
{
  UINT32 rfbSym;
  if (isPressed(modifier)) {
    bool success = m_keyMap.virtualCodeToKeySym(&rfbSym, modifier);
    _ASSERT(success);
    sendKeySymEvent(rfbSym, true);
  }
}

void RfbKeySym::checkAndSendDiff(unsigned char virtKey, unsigned char state)
{
  bool testedState = (state & 128) != 0;
  m_viewerKeyState[virtKey] = testedState ? 128 : 0;
  virtKey = distinguishLeftRightModifier(virtKey, false);

  bool srvState = (m_serverKeyState[virtKey] & 128) != 0;
  m_serverKeyState[virtKey] = testedState ? 128 : 0;

  if (testedState != srvState) { 
    UINT32 rfbSym;
    bool success = m_keyMap.virtualCodeToKeySym(&rfbSym, virtKey);
    _ASSERT(success);
    sendKeySymEvent(rfbSym, testedState);
  }
}

void RfbKeySym::clearKeyState()
{
  memset(m_viewerKeyState, 0, sizeof(m_viewerKeyState));
  memset(m_serverKeyState, 0, sizeof(m_serverKeyState));
}

bool RfbKeySym::isPressed(unsigned char virtKey)
{
  return (m_serverKeyState[virtKey] & 128) != 0;
}

void RfbKeySym::sendKeySymEvent(unsigned int rfbKeySym, bool down)
{
  // Translate Alt to Meta if Scroll Lock is on.
  if (rfbKeySym == XK_Alt_L || rfbKeySym == XK_Alt_R) {
    bool scrollLockOn = (GetKeyState(VK_SCROLL) & 1) != 0;
    if (scrollLockOn) {
      if (rfbKeySym == XK_Alt_L) {
        rfbKeySym = XK_Meta_L;
        m_leftMetaIsPressed = down;
      } else { // assuming (rfbKeySym == XK_Alt_R)
        rfbKeySym = XK_Meta_R;
        m_rightMetaIsPressed = down;
      }
    }
  }

  // Send the keyboard event.
  sendVerbatimKeySymEvent(rfbKeySym, down);
}

void RfbKeySym::sendVerbatimKeySymEvent(unsigned int rfbKeySym, bool down)
{
  m_extKeySymListener->onRfbKeySymEvent(rfbKeySym, down);
}

unsigned char RfbKeySym::distinguishLeftRightModifier(unsigned char virtKey,
                                                      bool isRightHint)
{
  if (virtKey == VK_CONTROL) {
    virtKey = isRightHint ? VK_RCONTROL : VK_LCONTROL;
  }
  if (virtKey == VK_MENU) {
    virtKey = isRightHint ? VK_RMENU : VK_LMENU;
  }
  if (virtKey == VK_SHIFT) {
    virtKey = isRightHint ? VK_RSHIFT : VK_LSHIFT;
  }
  return virtKey;
}
