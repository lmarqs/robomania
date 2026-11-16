#include "esc_motor.h"

// The minimum pulse sits at both the low end and the centre of the range, so throttles 0 to 1 span it and a
// negative one stays at the minimum.
EscMotor::EscMotor(PwmOutPin* pwm, uint32_t minPulseUs, uint32_t maxPulseUs)
    : out(pwm, PulseRange(minPulseUs, minPulseUs, maxPulseUs)) {}

bool EscMotor::begin() {
  const bool ready = out.begin();
  out.write(0.0f);
  return ready;
}

void EscMotor::write(float throttle) { out.write(throttle); }

float EscMotor::pulseUs(float throttle) const { return out.pulseUs(throttle); }
