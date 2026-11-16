#include "direction_pwm_motor.h"

DirectionPwmMotor::DirectionPwmMotor(PwmOutPin* enable, DigitalOutPin* in1, DigitalOutPin* in2)
    : enable(enable), in1(in1), in2(in2) {}

bool DirectionPwmMotor::begin() {
  // A bitwise &, so every pin starts even when an earlier one fails.
  const bool ready = in1->begin() & in2->begin() & enable->begin();
  write(0.0f);
  return ready;
}

void DirectionPwmMotor::write(float throttle) {
  in1->write(throttle > 0.0f);
  in2->write(throttle < 0.0f);
  enable->write(PwmOutPin::duty(throttle < 0.0f ? -throttle : throttle));
}
