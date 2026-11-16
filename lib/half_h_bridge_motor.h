#pragma once

#include "motor.h"
#include "pwm_out_pin.h"

/**
 * A DC motor on a driver with one PWM input per direction, such as the DRV8833 or the L9110S: `in1`'s duty drives it
 * forward and `in2`'s backward. At 0 both inputs go low and the motor coasts.
 */
class HalfHBridgeMotor : public Motor {
 private:
  PwmOutPin* in1;
  PwmOutPin* in2;

 public:
  HalfHBridgeMotor(PwmOutPin* in1, PwmOutPin* in2);

  bool begin() override;
  void write(float throttle) override;
};
