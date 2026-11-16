#include "reversible_esc_motor.h"

ReversibleEscMotor::ReversibleEscMotor(PwmOutPin* pwm, uint32_t minPulseUs, uint32_t neutralPulseUs,
                                       uint32_t maxPulseUs)
    : out(pwm, PulseRange(minPulseUs, neutralPulseUs, maxPulseUs)) {}

bool ReversibleEscMotor::begin() {
  const bool ready = out.begin();
  out.write(0.0f);
  return ready;
}

void ReversibleEscMotor::write(float throttle) { out.write(throttle); }

float ReversibleEscMotor::pulseUs(float throttle) const { return out.pulseUs(throttle); }
