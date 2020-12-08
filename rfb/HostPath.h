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

#ifndef __RFB_HOST_PATH_H_INCLUDED__
#define __RFB_HOST_PATH_H_INCLUDED__

// FIXME: Convert to TCHAR.

class HostPath {
public:
  HostPath();
  HostPath(const char *path, int defaultPort = 5900);
  virtual ~HostPath();

  bool set(const char *path);

  bool isValid() const               { return (m_path != 0); }
  bool isSshHostSpecified() const    { return (m_sshHost != 0); }

  const char* get() const            { return m_path; }
  const char* getSshHost() const     { return m_sshHost; }
  const int getSshPort() const       { return m_sshPort; }
  const char* getVncHost() const     { return m_vncHost; }
  const int getVncPort() const       { return m_vncPort; }

private:
  static const size_t m_SSH_USER_MAX_CHARS;
  static const size_t m_SSH_HOST_MAX_CHARS;
  static const size_t m_SSH_PORT_MAX_CHARS;
  static const size_t m_VNC_HOST_MAX_CHARS;
  static const size_t m_VNC_PORT_MAX_CHARS;
  static const size_t m_MAX_PATH_LEN;

  char* m_path;
  char* m_sshHost;
  int m_sshPort;
  char* m_vncHost;
  int m_vncPort;

  //
  // Reset the object to its initial state (no path set).
  //
  void clear();

  //
  // Parse m_path[] and store lengths if tokens in the specified array
  // of four size_t elements. Up to four tokens are detected: SSH host
  // name (optionally including a user name), SSH port number, VNC
  // host name, VNC port or display number. Port number tokens include
  // colons at the their beginning. Note that the '/' character
  // between SSH-related and VNC-related parts is not counted in any
  // token length.
  //
  // Examples: "user@server:22/vnc::443"  ->  11, 3, 3, 5
  //           "host/:1"                  ->   4, 0, 0, 2
  //           ":1"                       ->   0, 0, 0, 2
  //
  void parsePath(size_t results[]) const;

  //
  // Return true if m_sshHost[] and m_vncHost[] strings are valid,
  // false otherwise. It checks the lengths and character sets of the
  // strings. m_sshHost may be a null pointer, that does not make the
  // function return false. However, m_vncHost[] string is mandatory
  // so the function will return false if m_vncHost is a null pointer.
  //
  bool validateHostNames() const;

  int m_defaultPort;
};

#endif // __RFB_HOST_PATH_H_INCLUDED__
