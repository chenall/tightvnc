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

#ifndef _CONNECTION_CONFIG_H_
#define _CONNECTION_CONFIG_H_

#include "config-lib/SettingsManager.h"

#include "thread/LocalMutex.h"

#include "rfb/EncodingDefs.h"

//
// Contains options of connection configuration.
// All methods of class are thead-safe.
//

class ConnectionConfig
{
public:
  static const int NO_CURSOR = 0;
  static const int DOT_CURSOR = 1;
  static const int NORMAL_CURSOR = 2;
  static const int SMALL_CURSOR = 3;

public:
  ConnectionConfig();
  ConnectionConfig(const ConnectionConfig &conConfig);
  ~ConnectionConfig();
  ConnectionConfig& operator=(const ConnectionConfig& other);

  // Allow/disallow usage of encoding "copy rect"
  void allowCopyRect(bool allow);
  // Returns true if encoding "copy rect" is allowed to use
  bool isCopyRectAllowed();

  // Sets prefered encoding
  void setPreferredEncoding(int encoding);

  // Returns prefered encoding
  int getPreferredEncoding();

  // Sets 8 bit flag
  void use8BitColor(bool use);

  // Returns value of 8 bit use flag
  bool isUsing8BitColor();

  //
  // Compression members access methods
  //

  // Sets custom compression level.
  // Remark: if level is equals to -1 than custom compression
  // will be disabled.
  void setCustomCompressionLevel(int level);

  // Sets jpeg compression level.
  // Remark: if level is equals to -1 than custom compression
  // will be disabled.
  void setJpegCompressionLevel(int level);

  // Returns custom compression level or -1 if custom compression is disabled.
  int getCustomCompressionLevel();
  // Returns jpeg compression level or -1 if jpeg compression is disabled.
  int getJpegCompressionLevel();

  bool isCustomCompressionEnabled();
  bool isJpegCompressionEnabled();

  // Sets custom compression level to -1
  void disableCustomCompression();
  // Sets jpeg compression level to -1
  void disableJpegCompression();

  // Sets view only flag
  void setViewOnly(bool viewOnly);
  // Returns view only flag
  bool isViewOnly();

  // Enables/disables clipboard usage
  void enableClipboard(bool enabled);
  // Return true if clipboard usage is enabled
  bool isClipboardEnabled();

  // Sets fullscreen mode flag
  void enableFullscreen(bool useFullscreen);
  // Returns fullscreen flag
  bool isFullscreenEnabled();

  // Sets m_deiconifyOnRemoteBell member
  void deiconifyOnRemoteBell(bool deiconifyFlag);
  // Returns m_deiconifyOnRemoteBell member
  bool isDeiconifyOnRemoteBellEnabled();

  // Returns scale numerator
  int getScaleNumerator();
  // Returns scale denominator
  int getScaleDenominator();

  // Sets scale in percent relative to original
  void setScale(int n, int d);

  //
  // Mouse members access methods
  //

  // Sets swap mouse flag
  void swapMouse(bool enabled);
  // Returns swap mouse flag
  bool isMouseSwapEnabled();

  //
  // Other class members access methods
  //

  // Sets "shared" flag
  void setSharedFlag(bool shared);
  // Returns "shared" flag
  bool getSharedFlag();

  // Sets "fit window" flag
  void fitWindow(bool enabled);
  // Returns "fit window" flag
  bool isFitWindowEnabled();

  // Sets request shape updates flag
  void requestShapeUpdates(bool requestFlag);
  // Sets ignore shape updates flag
  void ignoreShapeUpdates(bool ignoreFlag);

  // Returns request shape updates flag
  bool isRequestingShapeUpdates();
  // Return ignore shape updates flag
  bool isIgnoringShapeUpdates();

  // Sets local cursor shape
  void setLocalCursorShape(int cursorShape);
  // Gets local cursor shape
  int getLocalCursorShape();

  //
  // Serialization / deserialization methods
  //

  // Serialize to storage this ConnectionConfig object.
  // Return true if was no errors during serializing,
  // false otherwise.
  bool saveToStorage(SettingsManager *sm) const;
  // Deserialize connection configuration from storage.
  // Return true if was no errors during deserializing,
  // false otherwise.
  bool loadFromStorage(SettingsManager *sm);
private:
  // FIXME: Move this method away from here.
  int gcd(int a, int b) const;

protected:

  //
  // "Format and encoding" group members
  //

  // Allowed "copy rect" encoding
  bool m_allowedCopyRect;
  // Preferred encoding
  unsigned char m_preferredEncoding;
  // Use 8bit color (for low-speed connections)
  bool m_use8BitColor;
  // Custom compression level
  int m_customCompressionLevel;
  // Jpeg compression level
  int m_jpegCompressionLevel;

  //
  // "Restrictions" group members
  //

  // View only mode (all client input are ignored)
  bool m_viewOnly;
  // Enable clipboard transfers flag
  bool m_isClipboardEnabled;

  //
  // "Display" group members
  //

  // If set then use fullscreen are to display remote display by default
  bool m_useFullscreen;
  // If set then deiconify window and play sound
  bool m_deiconifyOnRemoteBell;
  // If set then use autoscale to scale remote display image
  bool m_useAutoScale;
  // Scale numerator
  int m_scaleNumerator;
  // Scale denominator
  int m_scaleDenominator;

  //
  // "Mouse" members group
  //

  // Swap mouse buttons 2 and 3
  bool m_swapMouse;

  //
  // Non-grouped config option members
  //

  // If set than client will request shared session
  bool m_requestSharedSession;

  bool m_fitWindow;
  bool m_requestShapeUpdates;
  bool m_ignoreShapeUpdates;

  int m_localCursor;

  // Critical section
  mutable LocalMutex m_cs;
};

#endif
