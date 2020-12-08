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
#include "util/AnsiStringStorage.h"
#include "thread/AutoLock.h"

ClipboardExchange::ClipboardExchange(RfbCodeRegistrator *codeRegtor,
                                     Desktop *desktop,
                                     RfbOutputGate *output,
                                     bool viewOnly,
                                     LogWriter *log)
: m_desktop(desktop),
  m_output(output),
  m_viewOnly(viewOnly),
  m_hasNewClip(false),
  m_log(log)
{
  // Request code
  codeRegtor->regCode(ClientMsgDefs::CLIENT_CUT_TEXT, this);
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
    {
      UINT32 length = input->readUInt32();
      std::vector<char> charBuff(length + 1);

      input->readFully(&charBuff.front(), length);
      charBuff[length] = '\0';
      AnsiStringStorage ansiText(&charBuff.front());

      if (!m_viewOnly) {
        StringStorage clipText;
        ansiText.toStringStorage(&clipText);
        m_desktop->setNewClipText(&clipText);
      }
    }
    break;
  default:
    StringStorage errMess;
    errMess.format(_T("Unknown %d protocol code received"), (int)reqCode);
    throw Exception(errMess.getString());
    break;
  }
}

void ClipboardExchange::sendClipboard(const StringStorage *newClipboard)
{
  AutoLock al(&m_storedClipMut);
  if (!m_storedClip.isEqualTo(newClipboard)) {
    m_storedClip = *newClipboard;
    m_hasNewClip = true;
    m_newClipWaiter.notify();
  }
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
        AutoLock al(m_output);
        m_output->writeUInt8(ServerMsgDefs::SERVER_CUT_TEXT); // type
        m_output->writeUInt8(0); // pad
        m_output->writeUInt16(0); // pad

        AnsiStringStorage charBuff;
        {
          AutoLock al(&m_storedClipMut);
          charBuff.fromStringStorage(&m_storedClip);
          m_hasNewClip = false;
        }
        size_t strLength = charBuff.getLength();
        m_output->writeUInt32((UINT32)strLength);
        m_output->writeFully(charBuff.getString(), strLength);

        m_output->flush();
      } catch (Exception &e) {
        m_log->error(_T("The clipboard thread force to terminate because")
                   _T(" it caught the error: %s"), e.getMessage());
        terminate();
      }
    }
  }
}
