#pragma once

#include <stdint.h>

#include "motor.h"
#include "pulse_out.h"
#include "pwm_out_pin.h"

/**
 * An electronic speed controller that drives forward and in reverse, such as an RC car's, driven by pulse width on a
 * PWM pin, such as a BoardPwmOutPin at 50 Hz. Throttle 0 is the neutral pulse, which stops the motor; a longer one
 * drives forward and a shorter one brakes or reverses, as the ESC decides.
 */
class ReversibleEscMotor : public Motor {
 private:
  PulseOut out;

 public:
  explicit ReversibleEscMotor(PwmOutPin* pwm, uint32_t minPulseUs = 1000, uint32_t neutralPulseUs = 1500,
                              uint32_t maxPulseUs = 2000);

  /**
   * Starts the pin at the neutral pulse, which the ESC must see to arm, so the motor never runs at power-on.
   * Returns false when the pin cannot run.
   */
  bool begin() override;

  /** Sets the throttle, from -1 (the minimum pulse, full reverse) through 0 (neutral) to 1 (full forward). */
  void write(float throttle) override;

  /** The pulse width, in microseconds, that write() sends for `throttle`. */
  float pulseUs(float throttle) const;
};
