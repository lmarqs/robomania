#pragma once

#include <stdint.h>

/**
 * A pin that times the high pulses it receives, such as an RC receiver channel's. It reports a width, not a duty: it
 * does not know the period, and a servo-style signal carries its meaning in the width alone.
 */
class PulseInPin {
 public:
  /** Returns false when the pin cannot run, such as when it has no interrupt. */
  virtual bool begin() = 0;
  /** The width, in microseconds, of the last high pulse if it ended within the last `maxAgeUs`; otherwise 0. */
  virtual uint32_t read(uint32_t maxAgeUs) = 0;
};
