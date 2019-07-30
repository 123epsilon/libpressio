#include <cstdint>
#include "pressio_dtype.h"

extern "C" {
int pressio_dtype_size (enum pressio_dtype dtype) {
  switch(dtype) {
    case pressio_double_dtype:
      return sizeof(double);
    case pressio_float_dtype:
      return sizeof(float);
    case pressio_uint8_dtype:
      return sizeof(uint8_t);
    case pressio_uint16_dtype:
      return sizeof(uint16_t);
    case pressio_uint32_dtype:
      return sizeof(uint32_t);
    case pressio_uint64_dtype:
      return sizeof(uint64_t);
    case pressio_int8_dtype:
      return sizeof(int8_t);
    case pressio_int16_dtype:
      return sizeof(int16_t);
    case pressio_int32_dtype:
      return sizeof(int32_t);
    case pressio_int64_dtype:
      return sizeof(int64_t);
    case pressio_byte_dtype:
      return sizeof(unsigned char);
    default:
      return -1;
  }
}
}
