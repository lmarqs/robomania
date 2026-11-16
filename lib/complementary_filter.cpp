#include "complementary_filter.h"

ComplementaryFilter::ComplementaryFilter(float alpha) : alpha(alpha) {}

void ComplementaryFilter::reset(float angle) {
  estimate = angle;
  seeded = true;
}

float ComplementaryFilter::update(float absoluteAngle, float rate, float dtS) {
  if (!seeded) {
    reset(absoluteAngle);
    return estimate;
  }
  estimate = alpha * (estimate + rate * dtS) + (1.0f - alpha) * absoluteAngle;
  return estimate;
}

float ComplementaryFilter::angle() const { return estimate; }
