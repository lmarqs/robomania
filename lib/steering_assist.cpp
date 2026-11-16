#include "steering_assist.h"

static float clampUnit(float value, float low) { return value < low ? low : value > 1.0f ? 1.0f : value; }

SteeringAssist::SteeringAssist(float fullLockDps) : fullLockDps(fullLockDps) {}

float SteeringAssist::steer(float driverSteer, float yawRateDps, float gain) const {
  const float counterSteer = clampUnit(gain, 0.0f) * yawRateDps / fullLockDps;
  return clampUnit(driverSteer - counterSteer, -1.0f);
}
