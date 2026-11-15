#pragma once

#include <stdint.h>

#include "pin.h"
#include "pulse_out.h"

/**
 * A servo that holds the angle each pulse width encodes, from 0 to `range` degrees, such as 180, driven by a PWM pin
 * such as an EspPwmOutPin at 50 Hz. The defaults, 544 µs at 0° and 2400 µs at `range`, are the Arduino Servo
 * library's.
 */
class PositionalServo {
 private:
  PulseOut out;
  uint16_t range;

 public:
  PositionalServo(PwmOutPin* pwm, uint16_t range = 180, uint32_t minPulseUs = 544, uint32_t maxPulseUs = 2400);

  /** Starts the pin and sends nothing, so the servo stays limp until the first write(). */
  bool begin();

  /** Turns to `angle` degrees, clamped to 0 to `range`. */
  void write(int32_t angle);

  /** The pulse width, in microseconds, that write() sends for `angle`. */
  float pulseUs(int32_t angle) const;
};

/**
 * A continuous-rotation (360°) servo: the pulse width sets speed and direction, and the middle pulse stops it.
 * `speed` runs from -100 (full reverse) to 100 (full forward).
 */
class ContinuousServo {
 private:
  PulseOut out;

 public:
  ContinuousServo(PwmOutPin* pwm, uint32_t minPulseUs = 1000, uint32_t maxPulseUs = 2000);

  /** Starts the pin and sends nothing, so the servo stays still until the first move(). */
  bool begin();

  void move(int32_t speed);
  void stop();
};
