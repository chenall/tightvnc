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

#include "HttpReply.h"

HttpReply::HttpReply(DataOutputStream *dataOutput)
: m_dataOutput(dataOutput)
{
}

HttpReply::~HttpReply()
{
}

void HttpReply::send200()
{
  const char HTTP_200[] = "HTTP/1.0 200 OK\r\n\r\n";

  m_dataOutput->writeFully(HTTP_200, strlen(HTTP_200));
}

void HttpReply::send404()
{
  const char HTTP_404 [] = "HTTP/1.0 404 Not Found\r\n\r\n"
                           "<HTML>\n"
                           "  <HEAD><TITLE>404 Not Found</TITLE></HEAD>\n"
                           "  <BODY>\n"
                           "    <H1>Not Found</H1>\n"
                           "    The requested file could not be found.\n"
                           "  </BODY>\n"
                           "</HTML>\n";

  m_dataOutput->writeFully(HTTP_404, strlen(HTTP_404));
}
