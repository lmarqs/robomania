#pragma once

#include <stdint.h>

#include "pulse_out.h"
#include "pwm_out_pin.h"

/**
 * A servo that holds the angle each pulse width encodes, from 0 to `rangeDeg` degrees, such as 180, driven by a PWM
 * pin such as a BoardPwmOutPin at 50 Hz. The defaults, 544 µs at 0° and 2400 µs at `rangeDeg`, are the Arduino Servo
 * library's.
 */
class PositionalServo {
 private:
  PulseOut out;
  float rangeDeg;

 public:
  explicit PositionalServo(PwmOutPin* pwm, float rangeDeg = 180.0f, uint32_t minPulseUs = 544,
                           uint32_t maxPulseUs = 2400);

  /** Starts the pin and sends nothing, so the servo stays limp until the first write(). */
  bool begin();

  /** Turns to `angleDeg`, clamped to 0 to `rangeDeg`. */
  void write(float angleDeg);

  /** The pulse width, in microseconds, that write() sends for `angleDeg`. */
  float pulseUs(float angleDeg) const;
};
