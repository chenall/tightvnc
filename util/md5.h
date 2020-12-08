/* MD5
 converted to C++ class by Frank Thilo (thilo@unix-ag.org)
 for bzflag (http://www.bzflag.org)

   based on:

   md5.h and md5.c
   reference implementation of RFC 1321

   Copyright (C) 1991-2, RSA Data Security, Inc. Created 1991. All
rights reserved.

License to copy and use this software is granted provided that it
is identified as the "RSA Data Security, Inc. MD5 Message-Digest
Algorithm" in all material mentioning or referencing this software
or this function.

License is also granted to make and use derivative works provided
that such works are identified as "derived from the RSA Data
Security, Inc. MD5 Message-Digest Algorithm" in all material
mentioning or referencing the derived work.

RSA Data Security, Inc. makes no representations concerning either
the merchantability of this software or the suitability of this
software for any particular purpose. It is provided "as is"
without express or implied warranty of any kind.

These notices must be retained in any copies of any part of this
documentation and/or software.

*/

#ifndef _MD5_H_
#define _MD5_H_

#include "util/inttypes.h"

class MD5
{
public:
  MD5();
  void update(const unsigned char *buf, UINT32 length);
  void update(const char *buf, UINT32 length);
  MD5& finalize();

  /**
   * Returns 16 byte MD5 hash.
   */
  UINT8 *getHash();

private:
  static const int BLOCKSIZE = 64;

  static void decode(UINT32 output[], const UINT8 input[], UINT32 len);
  static void encode(UINT8 output[], const UINT32 input[], UINT32 len);

  void init();

  void transform(const UINT8 block[BLOCKSIZE]);

private:

  //
  // low level logic operations
  //

  static UINT32 F(UINT32 x, UINT32 y, UINT32 z);
  static UINT32 G(UINT32 x, UINT32 y, UINT32 z);
  static UINT32 H(UINT32 x, UINT32 y, UINT32 z);
  static UINT32 I(UINT32 x, UINT32 y, UINT32 z);

  static UINT32 rotateLeft(UINT32 x, int n);

  static void FF(UINT32 &a, UINT32 b, UINT32 c, UINT32 d, UINT32 x, UINT32 s, UINT32 ac);
  static void GG(UINT32 &a, UINT32 b, UINT32 c, UINT32 d, UINT32 x, UINT32 s, UINT32 ac);
  static void HH(UINT32 &a, UINT32 b, UINT32 c, UINT32 d, UINT32 x, UINT32 s, UINT32 ac);
  static void II(UINT32 &a, UINT32 b, UINT32 c, UINT32 d, UINT32 x, UINT32 s, UINT32 ac);

private:
  // if finalized.
  bool m_finalized;
  // bytes that didn't fit in last 64 byte chunk.
  UINT8 m_buffer[BLOCKSIZE];
  // 64bit counter for number of bits (lo, hi).
  UINT32 m_count[2];
  // m_digest so far.
  UINT32 m_state[4];
  // the result.
  UINT8 m_digest[16];
};

#endif
