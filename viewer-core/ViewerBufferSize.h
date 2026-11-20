#ifndef VIEWER_BUFFER_SIZE_H
#define VIEWER_BUFFER_SIZE_H

#include "util/inttypes.h"
#include <limits.h>

namespace ViewerBufferSize {

inline bool fitsFrameBuffer(int width, int height, int bytesPerPixel)
{
  if (width < 0 || height < 0 || bytesPerPixel <= 0) {
    return false;
  }

  const UINT64 alignedWidth = (static_cast<UINT64>(width) + 3) & ~static_cast<UINT64>(3);
  const UINT64 alignedHeight = (static_cast<UINT64>(height) + 3) & ~static_cast<UINT64>(3);
  return alignedWidth * alignedHeight <= INT_MAX / bytesPerPixel;
}

inline bool fitsDecoderBuffer(int width, int height, int bytesPerPixel)
{
  if (width < 0 || height < 0 || bytesPerPixel <= 0) {
    return false;
  }

  return static_cast<UINT64>(width) * height <= INT_MAX / bytesPerPixel;
}

} // namespace ViewerBufferSize

#endif // VIEWER_BUFFER_SIZE_H
