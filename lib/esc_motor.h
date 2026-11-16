#pragma once

#include <stdint.h>

#include "motor.h"
#include "pulse_out.h"
#include "pwm_out_pin.h"

/**
 * An electronic speed controller for a brushless motor that spins one way, driven by pulse width on a PWM pin, such as
 * a BoardPwmOutPin at 200 Hz. Throttle 0 is the minimum pulse, which stops the motor, and so is any throttle below it.
 */
class EscMotor : public Motor {
 private:
  PulseOut out;

 public:
  explicit EscMotor(PwmOutPin* pwm, uint32_t minPulseUs = 1000, uint32_t maxPulseUs = 2000);

  /**
   * Starts the pin at the minimum pulse. An ESC arms only after it has seen that pulse for a few seconds, so the
   * sketch waits before the first write(). Returns false when the pin cannot run.
   */
  bool begin() override;

  /** Sets the throttle, from 0 (the minimum pulse, motor stopped) to 1 (the maximum pulse). */
  void write(float throttle) override;

  /** The pulse width, in microseconds, that write() sends for `throttle`. */
  float pulseUs(float throttle) const;
};
