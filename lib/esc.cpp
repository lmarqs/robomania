#include "esc.h"

Esc::Esc(PwmOutPin* pwm, uint32_t minPulseUs, uint32_t maxPulseUs) {
  this->pwm = pwm;
  this->minPulseUs = minPulseUs;
  this->maxPulseUs = maxPulseUs;
}

bool Esc::begin() {
  ready = pwm->begin();
  write(0.0f);
  return ready;
}

float Esc::pulseUs(float throttle) const {
  throttle = constrain(throttle, 0.0f, 1.0f);
  return minPulseUs + throttle * (maxPulseUs - minPulseUs);
}

static void writePulse(PwmOutPin* pwm, float pulseUs) {
  float duty = pulseUs * pwm->frequency() / 1000000.0f * PwmOutPin::DUTY_MAX;
  pwm->write((uint16_t)min(duty + 0.5f, (float)PwmOutPin::DUTY_MAX));
}

void Esc::write(float throttle) {
  if (!ready) {
    return;
  }

  writePulse(pwm, pulseUs(throttle));
}

ReversibleEsc::ReversibleEsc(PwmOutPin* pwm, uint32_t minPulseUs, uint32_t neutralPulseUs, uint32_t maxPulseUs) {
  this->pwm = pwm;
  this->minPulseUs = minPulseUs;
  this->neutralPulseUs = neutralPulseUs;
  this->maxPulseUs = maxPulseUs;
}

bool ReversibleEsc::begin() {
  ready = pwm->begin();
  write(0.0f);
  return ready;
}

float ReversibleEsc::pulseUs(float throttle) const {
  throttle = constrain(throttle, -1.0f, 1.0f);
  if (throttle >= 0.0f) {
    return neutralPulseUs + throttle * (maxPulseUs - neutralPulseUs);
  }
  return neutralPulseUs + throttle * (neutralPulseUs - minPulseUs);
}

void ReversibleEsc::write(float throttle) {
  if (!ready) {
    return;
  }

  writePulse(pwm, pulseUs(throttle));
}
