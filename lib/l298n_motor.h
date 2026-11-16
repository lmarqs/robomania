#pragma once

#include "digital_out_pin.h"
#include "motor.h"
#include "pwm_out_pin.h"

/**
 * A DC motor on one H-bridge of an L298N, or of a chip wired like it, with three pins: `in1` and `in2` set the
 * direction and the enable pin's duty sets how hard the motor turns. At throttle 0 both inputs go low and the motor
 * coasts.
 */
class L298nMotor : public Motor {
 private:
  PwmOutPin* enable;
  DigitalOutPin* in1;
  DigitalOutPin* in2;

 public:
  L298nMotor(PwmOutPin* enable, DigitalOutPin* in1, DigitalOutPin* in2);

  bool begin() override;
  void write(float throttle) override;
};
