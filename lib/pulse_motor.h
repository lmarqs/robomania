#pragma once

#include "motor.h"
#include "pulse_out.h"
#include "pulse_range.h"
#include "pwm_out_pin.h"

/**
 * A motor its driver runs from a servo-style pulse on a PWM pin, such as a BoardPwmOutPin at 50 Hz: throttle 0 sends
 * the range's center pulse, which stops it, and -1 and 1 its ends. The range says which motor it is:
 *
 * - an ESC that reverses, such as an RC car's: `PulseRange(1000, 1500, 2000)`, neutral in the middle;
 * - a continuous-rotation (360°) servo: `PulseRange::linear(1000, 2000)`;
 * - a one-way ESC for a brushless motor, often at 200 Hz: `PulseRange::oneWay(1000, 2000)`, which stops for any
 *   throttle below 0.
 */
class PulseMotor : public Motor {
 private:
  PulseOut out;

 public:
  PulseMotor(PwmOutPin* pwm, PulseRange range);

  /**
   * Starts the pin at the stop pulse, which an ESC must see for a few seconds before it arms, so the motor never runs
   * at power-on and the sketch waits before the first write(). Returns false when the pin cannot run.
   */
  bool begin() override;
  void write(float throttle) override;

  /** The pulse width, in microseconds, that write() sends for `throttle`. */
  float pulseUs(float throttle) const;
};
