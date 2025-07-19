#ifndef QOI_HEADER_H
#define QOI_HEADER_H

#include <cstdint>

struct qoi_header{
  char magic[4];
  uint32_t width;
  uint32_t height;
  uint8_t channels;
  uint8_t colorspace;
};

#endif
