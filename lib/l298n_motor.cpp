#include "l298n_motor.h"

L298nMotor::L298nMotor(PwmOutPin* enable, DigitalOutPin* in1, DigitalOutPin* in2)
    : enable(enable), in1(in1), in2(in2) {}

bool L298nMotor::begin() {
  // A bitwise &, so every pin starts even when an earlier one fails.
  const bool ready = in1->begin() & in2->begin() & enable->begin();
  write(0.0f);
  return ready;
}

void L298nMotor::write(float throttle) {
  in1->write(throttle > 0.0f);
  in2->write(throttle < 0.0f);
  enable->write(PwmOutPin::duty(throttle < 0.0f ? -throttle : throttle));
}
