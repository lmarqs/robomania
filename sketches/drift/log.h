#pragma once

#include "./config.h"

// Its arguments are not evaluated while DRIFT_LOG is 0, so a disabled log costs nothing in the loop.
#define LOG(...)                  \
  do {                            \
    if (DRIFT_LOG) {              \
      Serial.printf(__VA_ARGS__); \
    }                             \
  } while (0)
