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

#ifndef __RFB_ENCODER_STORE_H_INCLUDED__
#define __RFB_ENCODER_STORE_H_INCLUDED__

#include <map>

#include "Encoder.h"
#include "JpegEncoder.h"

// EncoderStore is an object which allocates encoders on demand and serves
// callers with a pointer to currectly selected encoder. The goal of
// introducing this class was to guarantee that each encoder is allocated only
// once per one RFB client so that the states of all encoders would be
// preserved between multiple selections of the same encoder type (this is
// especially important for encoders utilizing zlib compression, like Tight or
// ZRLE).
//
// In addition to normal encoders, JPEG encoder is maintained as well. JPEG
// encoder works via an existing Tight encoder and forces it to use lossy JPEG
// compression whenever possible.

class EncoderStore
{
public:
  // Constructor. Its arguments will be stored for constructing encoders.
  // Note that no encoders are created in the constructor, so getEncoder()
  // will return 0 if called right after the object creation. The caller must
  // call selectEncoder() explicitly to allocate encoders, even if that's Raw
  // encoder (implemented in the base Encoder class).
  EncoderStore(PixelConverter *pixelConverter, DataOutputStream *output);
  ~EncoderStore();

  // Get current (preferred) encoder if it was previously allocated by
  // selectEncoder().
  Encoder *getEncoder() const;
  // Get a pointer to JpegEncoder if it was previously allocated by
  // validateJpegEncoder().
  JpegEncoder *getJpegEncoder() const;

  void selectEncoder(int encType);
  void validateJpegEncoder();

protected:
  // This function makes sure the specified encoder is allocated and stored in
  // m_map. If it's already there, this function returns a pointer to the
  // existing encoder. If it's not there, it will be allocated, stored in
  // m_map and the pointer to it will be returned. If the encoding type is
  // unknown, Raw encoder will be assumed.
  Encoder *validateEncoder(int encType);

  // Check if the encoding type code is supported.
  static bool encodingSupported(int encType);

  // Allocates the specified encoder (using new) and returns the pointer.
  // If the encoding type is unknown, throws an Exception.
  Encoder *allocateEncoder(int encType) const;

protected:
  // Map of encoders. Encoding codes are used as keys for dynamically
  // allocated objects derived from Encoder. Encoders are allocated by the
  // selectEncoder() function. Allocated encoders will not be deleted until
  // EncoderStore destruction.
  std::map<int, Encoder *> m_map;

  // Current encoder (preferred encoder). Initialized with a null pointer,
  // should point to the encoder selected by the most recent selectEncoder()
  // call. This pointer copies a pointer from the corresponding m_map record,
  // so the destructor should deallocate the encoder together with that m_map
  // record.
  Encoder *m_encoder;
  // Video-specific encoder. Initialized with a null pointer, allocated by the
  // validateJpegEncoder() method. This member variable is the only pointer to
  // JpegEncoder, there is no record in m_map for it, so this pointer should
  // be used to delete JpegEncoder on destruction.
  JpegEncoder *m_jpegEncoder;

  // This pointer to PixelConverter will be used to construct encoders.
  PixelConverter *m_pixelConverter;
  // This pointer to DataOutputStream will be used to construct encoders.
  DataOutputStream *m_output;

private:
  // Do not allow copying objects.
  EncoderStore(const EncoderStore &other);
  EncoderStore &operator=(const EncoderStore &other);
};

#endif // __RFB_ENCODER_STORE_H_INCLUDED__
