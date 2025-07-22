#ifndef QOI_READER_H
#define QOI_READER_H

#include "qoi_header.h"
#include <cstdint>

enum QOI_OP{
  RGB,
  RGBA,
  INDEX,
  DIFF,
  LUMA,
  RUN,
  MAX_TAG
};

bool read_qoi(const char* filename, uint32_t* data, Qoi_Header* header);

inline uint32_t arr_to_px(const char* buf);

inline int calc_index_hash(uint32_t pixel);

QOI_OP read_tag(const uint8_t tag);

#endif
