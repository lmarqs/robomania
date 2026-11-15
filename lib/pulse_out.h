#pragma once

#include <stdint.h>

#include "pin.h"
#include "pulse_width.h"

/**
 * A servo-style pulse on a PWM pin, set by a position from -1 to 1: the one path from a position to a pulse to a duty
 * that Esc, ReversibleEsc, PositionalServo and ContinuousServo share. It converts in microseconds, so the pulse moves
 * in steps as fine as the pin's duty allows, such as 0.3 µs on a 16-bit pin at 50 Hz.
 */
class PulseOut {
 private:
  PwmOutPin* pwm;
  PulseRange range;
  bool ready = false;

 public:
  PulseOut(PwmOutPin* pwm, PulseRange range);

  /** Starts the pin and sends nothing yet. Returns false when the pin cannot run; write() then does nothing. */
  bool begin();

  /** Sends the pulse for `position`, from -1 at the range's low pulse through 0 at its centre to 1 at its high one. */
  void write(float position);

  /** The pulse width, in microseconds, that write() sends for `position`. */
  float pulseUs(float position) const;
};
