#include "pulse_range.h"

// Written so a NaN fails both comparisons and comes out as `low`, never as a NaN cast to an integer.
static float clamp(float value, float low, float high) { return value >= low ? (value <= high ? value : high) : low; }

PulseRange::PulseRange(float lowUs, float centerUs, float highUs) : lowUs(lowUs), centerUs(centerUs), highUs(highUs) {}

PulseRange PulseRange::linear(float lowUs, float highUs) { return PulseRange(lowUs, (lowUs + highUs) / 2.0f, highUs); }

float PulseRange::pulseUs(float position) const {
  if (position != position) {
    return centerUs;
  }
  position = clamp(position, -1.0f, 1.0f);
  if (position >= 0.0f) {
    return centerUs + position * (highUs - centerUs);
  }
  return centerUs + position * (centerUs - lowUs);
}

float PulseRange::position(float pulseUs) const {
  const float offset = pulseUs - centerUs;
  if (offset != offset || offset == 0.0f) {
    return 0.0f;
  }
  const bool towardHigh = (offset > 0.0f) == (highUs > centerUs);
  const float span = towardHigh ? highUs - centerUs : centerUs - lowUs;
  // A one-way range has no span on one side of its centre; any pulse there is past that end.
  if (span == 0.0f) {
    return towardHigh ? 1.0f : -1.0f;
  }
  return clamp(offset / span, -1.0f, 1.0f);
}
