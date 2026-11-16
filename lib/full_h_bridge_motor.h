#pragma once

#include "digital_out_pin.h"
#include "motor.h"
#include "pwm_out_pin.h"

/**
 * A DC motor on one channel of a full H-bridge driver such as the L298N: `in1` and `in2` set the direction and the
 * enable pin's duty sets the speed. At 0 both inputs go low and the motor coasts.
 */
class FullHBridgeMotor : public Motor {
 private:
  PwmOutPin* enable;
  DigitalOutPin* in1;
  DigitalOutPin* in2;

 public:
  FullHBridgeMotor(PwmOutPin* enable, DigitalOutPin* in1, DigitalOutPin* in2);

  bool begin() override;
  void write(float throttle) override;
};
