#pragma once

#include <stdint.h>

#include "motor.h"
#include "pulse_out.h"
#include "pwm_out_pin.h"

/**
 * A continuous-rotation (360°) servo, driven by a PWM pin such as a BoardPwmOutPin at 50 Hz: the pulse width sets
 * speed and direction, and the middle pulse stops it.
 */
class ContinuousServoMotor : public Motor {
 private:
  PulseOut out;

 public:
  explicit ContinuousServoMotor(PwmOutPin* pwm, uint32_t minPulseUs = 1000, uint32_t maxPulseUs = 2000);

  /** Starts the pin and sends nothing, so the servo stays still until the first write(). */
  bool begin() override;
  void write(float throttle) override;
};
