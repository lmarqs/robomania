#include "rc_channel.h"

static float clamp(float value, float low, float high) { return value < low ? low : value > high ? high : value; }

static const uint32_t SHORTEST_SERVO_PULSE_US = 500;
static const uint32_t LONGEST_SERVO_PULSE_US = 2500;

bool RcChannel::isServoPulse(uint32_t widthUs) {
  return widthUs >= SHORTEST_SERVO_PULSE_US && widthUs <= LONGEST_SERVO_PULSE_US;
}

RcChannel::RcChannel(uint32_t lowUs, uint32_t centerUs, uint32_t highUs, uint32_t deadZoneUs) {
  this->lowUs = lowUs;
  this->centerUs = centerUs;
  this->highUs = highUs;
  this->deadZoneUs = deadZoneUs;
}

float RcChannel::position(uint32_t pulseUs) const {
  const int32_t offset = (int32_t)pulseUs - centerUs;
  if (offset > -deadZoneUs && offset < deadZoneUs) {
    return 0.0f;
  }
  const bool towardHigh = (offset > 0) == (highUs > centerUs);
  const int32_t span = towardHigh ? highUs - centerUs : centerUs - lowUs;
  return clamp((float)offset / span, -1.0f, 1.0f);
}

float RcChannel::level(uint32_t pulseUs) const {
  return clamp((float)((int32_t)pulseUs - lowUs) / (highUs - lowUs), 0.0f, 1.0f);
}
