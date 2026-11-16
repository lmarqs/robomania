#pragma once

#include <stdint.h>

/**
 * A pin that reads a voltage, from 0 at ground to VALUE_MAX at the board's reference: every board's ADC is scaled to
 * the same range, whatever its resolution.
 */
class AnalogInPin {
 public:
  static const uint16_t VALUE_MAX = 65535;

  /** Returns false when the pin cannot run. */
  virtual bool begin() = 0;
  virtual uint16_t read() = 0;
};
