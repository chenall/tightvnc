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

#include "ClipboardExchange.h"
#include "rfb/MsgDefs.h"
#include "util/Utf8StringStorage.h"
#include "util/AnsiStringStorage.h"
#include "thread/AutoLock.h"
#include "rfb/VendorDefs.h"

ClipboardExchange::ClipboardExchange(RfbCodeRegistrator *codeRegtor,
                                     Desktop *desktop,
                                     RfbOutputGate *output,
                                     bool viewOnly,
                                     LogWriter *log)
: m_desktop(desktop),
  m_output(output),
  m_viewOnly(viewOnly),
  m_hasNewClip(false),
  m_isUtf8ClipboardEnabled(false),
  m_log(log)
{
  // Request code
  codeRegtor->regCode(ClientMsgDefs::CLIENT_CUT_TEXT, this);

  codeRegtor->addClToSrvCap(ClientMsgDefs::CLIENT_CUT_TEXT_UTF8, VendorDefs::TIGHTVNC, Utf8CutTextDefs::CLIENT_CUT_TEXT_UTF8_SIG);
  codeRegtor->addClToSrvCap(ClientMsgDefs::ENABLE_CUT_TEXT_UTF8, VendorDefs::TIGHTVNC, Utf8CutTextDefs::ENABLE_CUT_TEXT_UTF8_SIG);
  codeRegtor->addSrvToClCap(ServerMsgDefs::SERVER_CUT_TEXT_UTF8, VendorDefs::TIGHTVNC, Utf8CutTextDefs::SERVER_CUT_TEXT_UTF8_SIG);
  codeRegtor->regCode(ClientMsgDefs::CLIENT_CUT_TEXT_UTF8, this);
  codeRegtor->regCode(ClientMsgDefs::ENABLE_CUT_TEXT_UTF8, this);

  resume();
}

ClipboardExchange::~ClipboardExchange()
{
  terminate();
  wait();
}

void ClipboardExchange::onRequest(UINT32 reqCode, RfbInputGate *input)
{

  switch (reqCode) {
  case ClientMsgDefs::CLIENT_CUT_TEXT:
    input->readUInt8(); // pad
    input->readUInt16(); // pad
    onRequestWorker(false, input);
    break;
  case ClientMsgDefs::CLIENT_CUT_TEXT_UTF8:
    onRequestWorker(true, input);
    break;
  case ClientMsgDefs::ENABLE_CUT_TEXT_UTF8:
    m_isUtf8ClipboardEnabled = true;
    break;
  default:
    StringStorage errMess;
    errMess.format(_T("Unknown %d protocol code received"), (int)reqCode);
    throw Exception(errMess.getString());
    break;
  }
}
void ClipboardExchange::onRequestWorker(bool utf8flag, RfbInputGate *input)
{
  UINT32 length = input->readUInt32();

  std::vector<char> charBuff(length + 1);

  input->readFully(&charBuff.front(), length);
  charBuff[length] = '\0';
  if (m_viewOnly) {
    return;
  }

  StringStorage clipText;
  if (utf8flag) {
    m_log->debug(_T("UTF8 ClientCutText, payload length %d"), length);
    Utf8StringStorage utfText(&charBuff);
    utfText.toStringStorage(&clipText);
  }
  else
  {
    m_log->debug(_T("ClientCutText, payload length %d"), length);
    AnsiStringStorage ansiText(&charBuff.front());
    ansiText.toStringStorage(&clipText);
  }
  m_desktop->setNewClipText(&clipText);
}

void ClipboardExchange::sendClipboard(const StringStorage *newClipboard)
{
  AutoLock al(&m_storedClipMut);
  m_storedClip = *newClipboard;
  m_hasNewClip = true;
  m_newClipWaiter.notify();
}

void ClipboardExchange::onTerminate()
{
  m_newClipWaiter.notify();
}

void ClipboardExchange::execute()
{
  while (!isTerminating()) {
    m_newClipWaiter.waitForEvent();

    if (m_hasNewClip && !isTerminating() && !m_viewOnly) {

      try {
        const char * data;
        size_t length;
        AutoLock al(m_output);
        if (m_isUtf8ClipboardEnabled) {
          m_output->writeUInt32(ServerMsgDefs::SERVER_CUT_TEXT_UTF8); // type
          Utf8StringStorage charBuff;
          {
            AutoLock al(&m_storedClipMut);
            charBuff.fromStringStorage(&m_storedClip);
            m_hasNewClip = false;
          }
          data = charBuff.getString();
          length = charBuff.getLength();
		      m_log->debug(_T("Sending Utf8 Clipboard, payload length %d"), length);
          m_output->writeUInt32((UINT32)length);
          m_output->writeFully(data, length);
        }
        else {
          m_output->writeUInt8(ServerMsgDefs::SERVER_CUT_TEXT); // type
          m_output->writeUInt8(0); // pad
          m_output->writeUInt16(0); // pad
          AnsiStringStorage charBuff;
          {
            AutoLock al(&m_storedClipMut);
            charBuff.fromStringStorage(&m_storedClip);
            m_hasNewClip = false;
          }
          data = charBuff.getString();
          length = charBuff.getLength();
		      m_log->debug(_T("Sending Clipboard, payload length %d"), length);
          m_output->writeUInt32((UINT32)length);
          m_output->writeFully(data, length);
        }
        m_output->flush();
      } catch (Exception &e) {
        m_log->error(_T("The clipboard thread force to terminate because")
                   _T(" it caught the error: %s"), e.getMessage());
        terminate();
      }
    }
  }
}
