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

#ifndef __CLIPBOARDEXCHANGE_H__
#define __CLIPBOARDEXCHANGE_H__

#include "RfbDispatcherListener.h"
#include "RfbCodeRegistrator.h"
#include "desktop/Desktop.h"
#include "network/RfbOutputGate.h"
#include "log-writer/LogWriter.h"

class ClipboardExchange : public RfbDispatcherListener, public Thread
{
public:
  ClipboardExchange(RfbCodeRegistrator *codeRegtor, Desktop *desktop,
                    RfbOutputGate *output, bool viewOnly, LogWriter *log);
  virtual ~ClipboardExchange();

  void sendClipboard(const StringStorage *newClipboard);

protected:
  // Listen function
  virtual void onRequest(UINT32 reqCode, RfbInputGate *input);

  virtual void execute();
  virtual void onTerminate();

private:
  bool m_viewOnly;
  Desktop *m_desktop;
  RfbOutputGate *m_output;

  WindowsEvent m_newClipWaiter;

  StringStorage m_storedClip;
  bool m_hasNewClip;
  LocalMutex m_storedClipMut;

  LogWriter *m_log;
};

#endif // __CLIPBOARDEXCHANGE_H__
