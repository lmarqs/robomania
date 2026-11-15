#include "rc_channel.h"

static const uint32_t SHORTEST_SERVO_PULSE_US = 500;
static const uint32_t LONGEST_SERVO_PULSE_US = 2500;

bool RcChannel::isServoPulse(uint32_t widthUs) {
  return widthUs >= SHORTEST_SERVO_PULSE_US && widthUs <= LONGEST_SERVO_PULSE_US;
}

// `travel` runs straight from low to high, so its position, shifted to 0..1, is the level.
RcChannel::RcChannel(uint32_t lowUs, uint32_t centerUs, uint32_t highUs, uint32_t deadZoneUs)
    : stick(lowUs, centerUs, highUs),
      travel(lowUs, (lowUs + highUs) / 2.0f, highUs),
      centerUs(centerUs),
      deadZoneUs(deadZoneUs) {}

float RcChannel::position(uint32_t pulseUs) const {
  const float offset = pulseUs - centerUs;
  if (offset > -deadZoneUs && offset < deadZoneUs) {
    return 0.0f;
  }
  return stick.position(pulseUs);
}

float RcChannel::level(uint32_t pulseUs) const { return (travel.position(pulseUs) + 1.0f) / 2.0f; }
