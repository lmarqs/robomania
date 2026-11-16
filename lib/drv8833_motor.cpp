#include "drv8833_motor.h"

Drv8833Motor::Drv8833Motor(PwmOutPin* in1, PwmOutPin* in2) : in1(in1), in2(in2) {}

bool Drv8833Motor::begin() {
  // A bitwise &, so both pins start even when the first fails.
  const bool ready = in1->begin() & in2->begin();
  write(0.0f);
  return ready;
}

void Drv8833Motor::write(float throttle) {
  in1->write(PwmOutPin::duty(throttle));
  in2->write(PwmOutPin::duty(-throttle));
}
