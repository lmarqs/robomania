#include "pulse_motor.h"

PulseMotor::PulseMotor(PwmOutPin* pwm, PulseRange range) : out(pwm, range) {}

bool PulseMotor::begin() {
  const bool ready = out.begin();
  out.write(0.0f);
  return ready;
}

void PulseMotor::write(float throttle) { out.write(throttle); }

float PulseMotor::pulseUs(float throttle) const { return out.pulseUs(throttle); }
