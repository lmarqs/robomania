#pragma once

#include "motor.h"
#include "pwm_out_pin.h"

/**
 * A DC motor on one H-bridge of a DRV8833, or of a chip wired like it, such as the L9110S, with one PWM pin per
 * direction: `in1`'s duty drives it forward and `in2`'s backward. At throttle 0 both inputs go low and the motor
 * coasts.
 */
class Drv8833Motor : public Motor {
 private:
  PwmOutPin* in1;
  PwmOutPin* in2;

 public:
  Drv8833Motor(PwmOutPin* in1, PwmOutPin* in2);

  bool begin() override;
  void write(float throttle) override;
};
