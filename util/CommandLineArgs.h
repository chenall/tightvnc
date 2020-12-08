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

#ifndef __COMMANDLINEARGS_H__
#define __COMMANDLINEARGS_H__

#include "util/CommonHeader.h"
#include <vector>

// This class produce initial parse of a command line.
// Each word or expression will be saved at self index.
class CommandLineArgs
{
public:
  CommandLineArgs();
  virtual ~CommandLineArgs();

  // Copies internal argument vector to the out variable.
  void getArgVector(std::vector<StringStorage> *out) const;

protected:
  std::vector<StringStorage> m_args;
};

#endif // __COMMANDLINEARGS_H__
