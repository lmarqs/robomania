#pragma once

#include <stdint.h>

/** A pin that outputs a square wave at a fixed frequency; write() sets how much of each period it stays high. */
class PwmOutPin {
 public:
  /** The duty that keeps the pin high for the whole period; 0 keeps it low. */
  static const uint16_t DUTY_MAX = 65535;

  /** The duty for `level`, from 0 (low all period) to 1 (high all period), rounded and clamped; NaN gives 0. */
  static uint16_t duty(float level) {
    if (!(level > 0.0f)) {
      return 0;
    }
    if (level >= 1.0f) {
      return DUTY_MAX;
    }
    return (uint16_t)(level * DUTY_MAX + 0.5f);
  }

  /** Returns false when the pin cannot run, such as when no hardware channel is left. */
  virtual bool begin() = 0;
  virtual void write(uint16_t duty) = 0;
  /** The frequency the hardware runs at, in hertz, which can differ slightly from the one asked for. */
  virtual uint32_t frequencyHz() = 0;
};
