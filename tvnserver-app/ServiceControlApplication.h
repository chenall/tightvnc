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

#ifndef _SERVICE_CONTROL_APPLICATION_H_
#define _SERVICE_CONTROL_APPLICATION_H_

#include "util/CommonHeader.h"

#include "win-system/WindowsApplication.h"
#include "win-system/SCMClient.h"

#include "TvnService.h"
#include "ServiceControlCommandLine.h"

/**
 * Service control application.
 * Application that can install, remove, start and stop tvnserver service.
 */
class ServiceControlApplication : public WindowsApplication
{
public:
  /**
   * Creates new service control application.
   */
  ServiceControlApplication(HINSTANCE hInstance,
                            const TCHAR *windowClassName,
                            const TCHAR *commandLine);
  /**
   * Destructor.
   */
  virtual ~ServiceControlApplication();

  /**
   * Runs service control application.
   * @return application exit code.
   */
  virtual int run();

private:
  /**
   * Starts an instance of the same ServiceControlApplication with elevated
   * privileges and an additional -dontelevate command-line option (which is
   * used to avoid infinite elevation loop).
   * @throws SystemException on failure.
   */
  void runElevatedInstance() const throw(SystemException);
  /**
   * Perform the action specified in cmdLine (install, reinstall, remove,
   * start or stop the service).
   * @param cmdLine pointer to the parsed command line.
   * @throws SystemException on failure.
   */
  void executeCommand(const ServiceControlCommandLine *cmdLine) const
    throw(SystemException);
  /**
   * Writes tvncontrol entry to registry to start it in every session.
   * @throws SystemException when failed to write to registry.
   */
  // FIXME: Move this function to TvnService, call from TvnService::install().
  void setTvnControlStartEntry() const throw(SystemException);
  /**
   * Removes tvncontrol start entry from registry.
   * @throws SystemException when failed delete registry key.
   */
  // FIXME: Move this function to TvnService, call from TvnService::remove().
  void removeTvnControlStartEntry() const throw(SystemException);
  /**
   * Report an error using error description from a SCMClientException object.
   * @param cmdLine pointer to command line parser.
   * @param ex pointer to an SCMClientException object describing the error.
   * @remark does not show up a message box if the -silent flag was specified
   *   in the command line.
   */
  void reportError(const ServiceControlCommandLine *cmdLine,
                   const SCMClientException *ex) const;
  /**
   * Report an error using error description from a SystemException object.
   * @param cmdLine pointer to command line parser.
   * @param ex pointer to a SystemException object describing the error.
   * @remark does not show up a message box if the -silent flag was specified
   *   in the command line.
   */
  void reportError(const ServiceControlCommandLine *cmdLine,
                   const SystemException *ex) const;
  /**
   * Report an error using error description specified as a string.
   * @param cmdLine pointer to command line parser.
   * @param errorMessage error description.
   * @remark does not show up a message box if the -silent flag was specified
   *   in the command line.
   */
  void reportError(const ServiceControlCommandLine *cmdLine,
                   const TCHAR *errorMessage) const;
  /**
   * Report successful command execution.
   * @param cmdLine pointer to command line parser.
   * @remark does not show up a message box if the -silent flag was specified
   *   in the command line.
   */
  void reportSuccess(const ServiceControlCommandLine *cmdLine) const;

private:
  /**
   * Command line passed to the application constructor.
   */
  StringStorage m_commandLine;
};

#endif
