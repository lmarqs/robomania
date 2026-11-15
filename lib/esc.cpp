#include "esc.h"

// The minimum pulse sits at both the low end and the centre of the range, so throttles 0 to 1 span it and a
// negative one stays at the minimum.
Esc::Esc(PwmOutPin* pwm, uint32_t minPulseUs, uint32_t maxPulseUs) : range(minPulseUs, minPulseUs, maxPulseUs) {
  this->pwm = pwm;
}

bool Esc::begin() {
  ready = pwm->begin();
  write(0.0f);
  return ready;
}

float Esc::pulseUs(float throttle) const { return range.pulseUs(throttle); }

void Esc::write(float throttle) {
  if (!ready) {
    return;
  }

  pwm->write(pulseDuty(pulseUs(throttle), pwm->frequency()));
}

ReversibleEsc::ReversibleEsc(PwmOutPin* pwm, uint32_t minPulseUs, uint32_t neutralPulseUs, uint32_t maxPulseUs)
    : range(minPulseUs, neutralPulseUs, maxPulseUs) {
  this->pwm = pwm;
}

bool ReversibleEsc::begin() {
  ready = pwm->begin();
  write(0.0f);
  return ready;
}

float ReversibleEsc::pulseUs(float throttle) const { return range.pulseUs(throttle); }

void ReversibleEsc::write(float throttle) {
  if (!ready) {
    return;
  }

  pwm->write(pulseDuty(pulseUs(throttle), pwm->frequency()));
}
