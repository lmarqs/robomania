#include "pulse_width.h"

static float clamp(float value, float low, float high) { return value < low ? low : value > high ? high : value; }

uint16_t pulseDuty(float pulseUs, uint32_t frequencyHz) {
  const float duty = pulseUs * frequencyHz / 1000000.0f * PULSE_DUTY_MAX;
  return (uint16_t)(clamp(duty, 0.0f, PULSE_DUTY_MAX) + 0.5f);
}

PulseRange::PulseRange(float lowUs, float centerUs, float highUs) {
  this->lowUs = lowUs;
  this->centerUs = centerUs;
  this->highUs = highUs;
}

float PulseRange::pulseUs(float position) const {
  position = clamp(position, -1.0f, 1.0f);
  if (position >= 0.0f) {
    return centerUs + position * (highUs - centerUs);
  }
  return centerUs + position * (centerUs - lowUs);
}
