#include "half_h_bridge_motor.h"

HalfHBridgeMotor::HalfHBridgeMotor(PwmOutPin* in1, PwmOutPin* in2) : in1(in1), in2(in2) {}

bool HalfHBridgeMotor::begin() {
  // A bitwise &, so both pins start even when the first fails.
  const bool ready = in1->begin() & in2->begin();
  write(0.0f);
  return ready;
}

void HalfHBridgeMotor::write(float throttle) {
  in1->write(PwmOutPin::duty(throttle));
  in2->write(PwmOutPin::duty(-throttle));
}
