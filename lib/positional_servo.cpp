#include "positional_servo.h"

PositionalServo::PositionalServo(PwmOutPin* pwm, float rangeDeg, uint32_t minPulseUs, uint32_t maxPulseUs)
    : out(pwm, PulseRange::linear(minPulseUs, maxPulseUs)), rangeDeg(rangeDeg) {}

bool PositionalServo::begin() { return out.begin(); }

static float positionOf(float angleDeg, float rangeDeg) { return 2.0f * angleDeg / rangeDeg - 1.0f; }

void PositionalServo::write(float angleDeg) { out.write(positionOf(angleDeg, rangeDeg)); }

float PositionalServo::pulseUs(float angleDeg) const { return out.pulseUs(positionOf(angleDeg, rangeDeg)); }
