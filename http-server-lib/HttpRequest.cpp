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

#include "HttpRequest.h"

HttpRequest::HttpRequest(DataInputStream *dataInput)
: m_dataInput(dataInput), m_argList(NULL)
{
  memset(m_request, 0, sizeof(m_request));
  memset(m_filename, 0, sizeof(m_filename));
  memset(m_args, 0, sizeof(m_args));
}

HttpRequest::~HttpRequest()
{
  if (m_argList != NULL) {
    delete m_argList;
  }
}

const char *HttpRequest::getRequest() const
{
  return m_request;
}

const char *HttpRequest::getFilename() const
{
  return m_filename;
}

bool HttpRequest::hasArguments() const
{
  return (m_argList != NULL) && (m_argList->getCount() > 0);
}

ArgList *HttpRequest::getArguments() const
{
  return m_argList;
}

void HttpRequest::readHeader()
{
  readLine('\n', m_request, sizeof(m_request) - 1);

  skipHeader(true);
}

void HttpRequest::skipHeader(bool lastWasEndLn)
{
  char c;

  for (;;) {
    m_dataInput->readFully(&c, 1);

    if (c == '\n') {
      if (lastWasEndLn) {
        break;
      }
      lastWasEndLn = true;
    } else {
      if (c >= ' ') {
        lastWasEndLn = false;
      }
    }
  }
}

bool HttpRequest::parseHeader()
{
  if (m_argList != NULL) {
    delete m_argList;
    m_argList = NULL;
  }
  _ASSERT(strnlen(m_request, 2048) < 2048); // 2048 is m_filename size
  // Try to extract filename from request.
  if (sscanf(m_request, "GET %2000s HTTP/", m_filename) != 1) {
    return false;
  }

  //
  // Split filename and arguments.
  //

  size_t filenameLen = strlen(m_filename);

  size_t j = 0;

  bool wasDelimiter = false;

  for (size_t i = 0; i < filenameLen; i++) {
    if (m_filename[i] == '?') {
      m_filename[i] = '\0';
      wasDelimiter = true;
      continue ;
    }
    if (wasDelimiter) {
      m_args[j++] = m_filename[i];
    }
  }

  m_argList = new ArgList(m_args);

  return true;
}

void HttpRequest::readLine(char endLnChar, char *buffer, size_t maxSize)
{
  size_t readTotal = 0;

  char c = 0;

  while (readTotal < maxSize) {
    try {
      m_dataInput->readFully(&c, 1);
    } catch (IOException &ioEx) {
      throw ioEx;
    } // try / catch.

    buffer[readTotal++] = c;

    if (c == endLnChar) {
      break;
    } // if read char is delimitter.
  } // while

  buffer[readTotal] = '\0';
}
