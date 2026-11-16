#pragma once

#include <stdint.h>

#include "pulse_range.h"

/**
 * One channel of an RC receiver: the servo pulse width it sends, mapped to where the transmitter's stick, trigger or
 * knob stands. `lowUs`, `centerUs` and `highUs` are the pulses at one end, the middle and the other end; `lowUs` above
 * `highUs` reverses the channel. It reads pulses through the same PulseRange that PulseOut sends them with, plus a dead
 * zone around the center.
 */
class RcChannel {
 private:
  PulseRange stick, travel;
  float centerUs, deadZoneUs;

 public:
  /** Whether `widthUs` is a servo pulse, 500 to 2500 µs; a receiver sends nothing else, so any other width is noise. */
  static bool isServoPulse(uint32_t widthUs);

  /** A pulse within `deadZoneUs` of `centerUs` reads as the center, so a stick that rests a little off still stops. */
  RcChannel(uint32_t lowUs, uint32_t centerUs, uint32_t highUs, uint32_t deadZoneUs);

  /** From -1 at `lowUs` through 0 at `centerUs` to 1 at `highUs`, each side scaled on its own and clamped. */
  float position(uint32_t pulseUs) const;

  /** From 0 at `lowUs` to 1 at `highUs`, clamped, for a knob or switch that sets a value rather than a direction. */
  float level(uint32_t pulseUs) const;
};
