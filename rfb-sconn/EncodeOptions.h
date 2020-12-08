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

#ifndef __RFB_ENCODE_OPTIONS_H_INCLUDED__
#define __RFB_ENCODE_OPTIONS_H_INCLUDED__

#include <vector>

class EncodeOptions
{
public:
  EncodeOptions();
  virtual ~EncodeOptions();

  // Reset all encoding-related options to defaults as if the client did not
  // send SetEncodings message.
  void reset();

  // Process the list of encodings requested by the RFB client.
  void setEncodings(std::vector<int> *list);

  // Return preferred encoding as previously set with setEncodings().
  // Preferred encoding is the first "normal" encoding from the list passed
  // to setEncodings(). If setEncodings() has not been called after object
  // construction or the most recent reset() call, then Raw will be considered
  // the preferred encoding so this function will return EncodingDefs::RAW.
  // Similarly, if there was no suitable encoding in the list passed in the
  // most recent setEncodings() call, EncodingDefs::RAW will be returned.
  int getPreferredEncoding() const;

  // Return true is a particular encoding was enabled via setEncodings(),
  // false otherwise. This function always returns true for the Raw encoding.
  // It should be used only for "normal" encodings (currently, that's Raw,
  // Hextile and Tight). It will return false for CopyRect and all
  // pseudo-encodings.
  bool encodingEnabled(int code) const;

  // Return the compression level in the range 0..9 if it was previously set
  // via setEncodings(). If it was not set, return the value of the
  // defaultLevel argument (which defaults to -1).
  int getCompressionLevel(int defaultLevel = EO_DEFAULT) const;

  // Return the JPEG quality level in the range 0..9 if it was previously set
  // via setEncodings(). If it was not set, return the value of the
  // defaultLevel argument (which defaults to -1).
  int getJpegQualityLevel(int defaultLevel = EO_DEFAULT) const;

  // Return true if JPEG quality level was previously set via setEncodings(),
  // false otherwise.
  bool jpegEnabled() const;

  // Disable JPEG for lossless compression
  void disableJpeg();

  //
  // Accessor functions to boolean values.
  //

  bool copyRectEnabled() const;
  bool richCursorEnabled() const;
  bool pointerPosEnabled() const;
  bool desktopSizeEnabled() const;

protected:

  // Return true if we know the specified encoding and it can be set as
  // preferred one, false otherwise. This function returns true only for
  // "normal" encodings (currently, that's Raw, Hextile and Tight). It will
  // return false for CopyRect, pseudo-encodings and unknown codes.
  static bool normalEncoding(int code);

  static const int EO_DEFAULT = -1;

  int m_preferredEncoding;

  // FIXME: Use something like std::map instead of individual variables.
  bool m_enableRRE;
  bool m_enableHextile;
  bool m_enableZrle;
  bool m_enableTight;

  int m_compressionLevel;
  int m_jpegQualityLevel;

  bool m_enableCopyRect;
  bool m_enableRichCursor;
  bool m_enablePointerPos;
  bool m_enableDesktopSize;
};

#endif // __RFB_ENCODE_OPTIONS_H_INCLUDED__
