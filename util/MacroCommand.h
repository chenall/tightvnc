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

#ifndef _MACRO_COMMAND_H_
#define _MACRO_COMMAND_H_

#include "Command.h"

#include <list>

/**
 * Command that executes sequence of commands.
 */
class MacroCommand : public Command
{
public:
  /**
   * Creates macro command with empty command list.
   */
  MacroCommand();
  /**
   * Destructor, does nothing.
   */
  virtual ~MacroCommand();
  /**
   * Executes macro command (sequence of commands that pushed to macro by
   * calling of addCommand() method).
   */
  virtual void execute();

  /**
   * Adds command to the end of list of commands to execute.
   */
  void addCommand(Command *command);

private:
  /**
   * List of commands to execute.
   */
  std::list<Command *> m_commandList;
};

#endif
