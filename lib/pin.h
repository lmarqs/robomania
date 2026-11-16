#pragma once

#include <stdint.h>

/**
 * The kinds of pin a driver can ask for. Each interface speaks the unit its hardware works in: a level, a PWM duty, or
 * a pulse width in microseconds. The adapters that implement them for a board live in `<kind>_pin.h`, such as
 * `digital_pin.h` or `pwm_out_pin.h`.
 */

class InPin {
 public:
  virtual void begin();
  virtual uint32_t read();
};

class OutPin {
 public:
  virtual void begin();
  virtual void write(uint32_t value);
};

/** A pin that outputs a square wave at a fixed frequency; write() sets how much of each period it stays high. */
class PwmOutPin {
 public:
  /** The duty that keeps the pin high for the whole period; 0 keeps it low. */
  static const uint16_t DUTY_MAX = 65535;

  /** Returns false when the pin cannot run, such as when no hardware channel is left. */
  virtual bool begin() = 0;
  virtual void write(uint16_t duty) = 0;
  /** The frequency the hardware runs at, in hertz, which can differ slightly from the one asked for. */
  virtual uint32_t frequencyHz() = 0;
};

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
