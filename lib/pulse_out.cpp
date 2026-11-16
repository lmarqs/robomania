#include "pulse_out.h"

PulseOut::PulseOut(PwmOutPin* pwm, PulseRange range) : pwm(pwm), range(range) {}

bool PulseOut::begin() {
  ready = pwm->begin();
  return ready;
}

void PulseOut::write(float position) {
  if (!ready) {
    return;
  }
  pwm->write(PwmOutPin::duty(range.pulseUs(position) * pwm->frequencyHz() / 1000000.0f));
}

float PulseOut::pulseUs(float position) const { return range.pulseUs(position); }
