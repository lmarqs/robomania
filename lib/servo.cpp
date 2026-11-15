#include "servo.h"

// A servo's travel is linear from the shortest pulse to the longest, so the centre is half way.
static PulseRange linearRange(uint32_t minPulseUs, uint32_t maxPulseUs) {
  return PulseRange(minPulseUs, (minPulseUs + maxPulseUs) / 2.0f, maxPulseUs);
}

PositionalServo::PositionalServo(PwmOutPin* pwm, uint16_t range, uint32_t minPulseUs, uint32_t maxPulseUs)
    : out(pwm, linearRange(minPulseUs, maxPulseUs)), range(range) {}

bool PositionalServo::begin() { return out.begin(); }

static float positionOf(int32_t angle, uint16_t range) { return 2.0f * angle / range - 1.0f; }

void PositionalServo::write(int32_t angle) { out.write(positionOf(angle, range)); }

float PositionalServo::pulseUs(int32_t angle) const { return out.pulseUs(positionOf(angle, range)); }

ContinuousServo::ContinuousServo(PwmOutPin* pwm, uint32_t minPulseUs, uint32_t maxPulseUs)
    : out(pwm, linearRange(minPulseUs, maxPulseUs)) {}

bool ContinuousServo::begin() { return out.begin(); }

void ContinuousServo::move(int32_t speed) { out.write(speed / 100.0f); }

void ContinuousServo::stop() { out.write(0.0f); }
