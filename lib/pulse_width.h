#pragma once

#include <stdint.h>

#include "pwm_out_pin.h"

/**
 * The duty, from 0 to PwmOutPin::DUTY_MAX, that makes a pin running at `frequencyHz` hold each pulse high for `pulseUs`
 * microseconds. It is rounded to the nearest step and clamped, so a pulse longer than the period keeps the pin high,
 * and a NaN pulse keeps it low.
 */
uint16_t pulseDuty(float pulseUs, uint32_t frequencyHz);

/**
 * The pulses at one end, the middle and the other end of a servo's or ESC's travel, or of an RC receiver channel's
 * stick. It converts both ways: pulseUs() gives the pulse to send for a position, and position() reads a received
 * pulse back as one. `lowUs` above `highUs` reverses the travel.
 */
class PulseRange {
 private:
  float lowUs, centerUs, highUs;

 public:
  PulseRange(float lowUs, float centerUs, float highUs);

  /** A range whose centre is half way between its ends, as a servo's travel is. */
  static PulseRange linear(float lowUs, float highUs);

  /**
   * The pulse, in microseconds, for `position` from -1 at `lowUs` through 0 at `centerUs` to 1 at `highUs`, each side
   * scaled on its own. A position past either end is clamped to it, and a NaN one gives `centerUs`.
   */
  float pulseUs(float position) const;

  /**
   * The position, from -1 at `lowUs` through 0 at `centerUs` to 1 at `highUs`, that `pulseUs` stands for: the inverse
   * of pulseUs(). A pulse past either end is clamped to it, and a NaN one gives 0.
   */
  float position(float pulseUs) const;
};
