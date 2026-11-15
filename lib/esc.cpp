#include "esc.h"

// The minimum pulse sits at both the low end and the centre of the range, so throttles 0 to 1 span it and a
// negative one stays at the minimum.
Esc::Esc(PwmOutPin* pwm, uint32_t minPulseUs, uint32_t maxPulseUs)
    : out(pwm, PulseRange(minPulseUs, minPulseUs, maxPulseUs)) {}

bool Esc::begin() {
  const bool ready = out.begin();
  out.write(0.0f);
  return ready;
}

void Esc::write(float throttle) { out.write(throttle); }

float Esc::pulseUs(float throttle) const { return out.pulseUs(throttle); }

ReversibleEsc::ReversibleEsc(PwmOutPin* pwm, uint32_t minPulseUs, uint32_t neutralPulseUs, uint32_t maxPulseUs)
    : out(pwm, PulseRange(minPulseUs, neutralPulseUs, maxPulseUs)) {}

bool ReversibleEsc::begin() {
  const bool ready = out.begin();
  out.write(0.0f);
  return ready;
}

void ReversibleEsc::write(float throttle) { out.write(throttle); }

float ReversibleEsc::pulseUs(float throttle) const { return out.pulseUs(throttle); }
