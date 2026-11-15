#pragma once

#include <stdint.h>

/** The duty that keeps a PWM pin high for the whole period, as PwmOutPin::DUTY_MAX. */
const uint16_t PULSE_DUTY_MAX = 65535;

/**
 * The duty, from 0 to PULSE_DUTY_MAX, that makes a pin running at `frequencyHz` hold each pulse high for `pulseUs`
 * microseconds. It is rounded to the nearest step and clamped, so a pulse longer than the period keeps the pin high.
 */
uint16_t pulseDuty(float pulseUs, uint32_t frequencyHz);

/**
 * The pulses at one end, the middle and the other end of a servo's or ESC's travel. `lowUs` above `highUs` reverses
 * the travel.
 */
class PulseRange {
 private:
  float lowUs, centerUs, highUs;

 public:
  PulseRange(float lowUs, float centerUs, float highUs);

  /**
   * The pulse, in microseconds, for `position` from -1 at `lowUs` through 0 at `centerUs` to 1 at `highUs`, each side
   * scaled on its own. A position past either end is clamped to it.
   */
  float pulseUs(float position) const;
};
