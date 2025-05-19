#pragma once
#include <cstdint>

struct Tick {
  int64_t timestamp;   // nanoseconds since epoch
  int64_t price;       // scaled 1 e-9
  int32_t size;
};
