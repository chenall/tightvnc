// Copyright (C) 2011,2012 GlavSoft LLC.
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

#include <algorithm>
#include <functional>

#include "DecoderStore.h"

DecoderStore::DecoderStore(LogWriter *logWriter)
: m_logWriter(logWriter),
  m_preferredEncoding(EncodingDefs::TIGHT),
  m_allowCopyRect(true)
{
}

DecoderStore::~DecoderStore()
{
  try {
    for (map<INT32, pair<int, Decoder *> >::iterator i = m_decoders.begin();
         i != m_decoders.end();
         i++) {
      m_logWriter->detail(_T("Decoder '%d' destroyed"), i->second.second->getCode());
      try {
        delete i->second.second;
      } catch (...) {
      }
    }
  } catch (...) {
  }
}

Decoder *DecoderStore::getDecoder(INT32 decoderId)
{
  if (m_decoders.count(decoderId))
    return m_decoders[decoderId].second;
  else
    return 0;
}

vector<INT32> DecoderStore::getDecoderIds()
{
  // this method returned list of decoders, sorted by priority.
  // in first position is preffered encoding.
  vector<pair<int, INT32> > decoders;

  for (map<INT32, pair <int, Decoder *> >::iterator i = m_decoders.begin();
       i != m_decoders.end();
       i++) {
    // preferred encoding is skipping
    if (i->first != m_preferredEncoding) {
      // copy rect is allowed?
      if (i->first != EncodingDefs::COPYRECT || m_allowCopyRect)
        decoders.push_back(make_pair(i->second.first, i->first));
    }
  }
  sort(decoders.begin(), decoders.end(), greater<pair<int,INT32> >());
  vector<INT32> sortedDecoders;
  map<INT32, pair<int, Decoder *> >::iterator priorityEnc = m_decoders.find(m_preferredEncoding);
  if (priorityEnc != m_decoders.end())
    sortedDecoders.push_back(priorityEnc->first);
  for (vector<pair<INT32, int> >::iterator i = decoders.begin();
       i != decoders.end();
       i++) {
    sortedDecoders.push_back(i->second);
  }
  if (sortedDecoders.empty())
    sortedDecoders.push_back(EncodingDefs::RAW);
  return sortedDecoders;
}

bool DecoderStore::addDecoder(Decoder *decoder, int priority)
{
  m_logWriter->detail(_T("Decoder %d added"), decoder->getCode());
  if (m_decoders.count(decoder->getCode()) == 0) {
    m_decoders[decoder->getCode()] = make_pair(priority, decoder);
    return true;
  }
  delete[] decoder;
  return false;
}

bool DecoderStore::removeDecoder(INT32 decoderId)
{
  if (m_decoders.count(decoderId)) {
    m_logWriter->detail(_T("Decoder '%d' destroyed (removed from list)"),
                        m_decoders[decoderId].second->getCode());
    delete m_decoders[decoderId].second;
    m_decoders.erase(decoderId);
    return true;
  }
  return false;
}

void DecoderStore::setPreferredEncoding(INT32 encodingType)
{
  m_logWriter->detail(_T("Decoder store: preferred encoding is \"%d\"."), encodingType);
  m_preferredEncoding = encodingType;
}

void DecoderStore::allowCopyRect(bool allow)
{
  if (allow) {
    m_logWriter->detail(_T("Decoder store: enable Copy Rect"));
  } else {
    m_logWriter->detail(_T("Decoder store: disable Copy Rect"));
  }
  m_allowCopyRect = allow;
}
