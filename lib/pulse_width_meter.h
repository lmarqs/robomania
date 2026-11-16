#pragma once

#include <stdint.h>

/**
 * Measures high pulses from the edges a pin interrupt reports, on any board: the logic BoardPulseInPin runs in its
 * interrupt handler. Its methods are always inlined, so a handler that must run from RAM, as on the ESP8266, never
 * calls into flash. The caller guards a copy against the handler, as BoardPulseInPin does.
 */
class PulseWidthMeter {
 private:
  bool high = false;
  bool hasPulse = false;
  uint32_t lastEdgeUs = 0;
  uint32_t lastWidthUs = 0;
  uint32_t lastEndUs = 0;

 public:
  /**
   * Records an edge to `high` at `nowUs`. A falling edge counts only after a rising one, so the first pulse after the
   * pin starts is never half of one.
   */
  __attribute__((always_inline)) inline void recordEdge(bool high, uint32_t nowUs) {
    if (!high && this->high) {
      lastWidthUs = nowUs - lastEdgeUs;
      lastEndUs = nowUs;
      hasPulse = true;
    }
    this->high = high;
    lastEdgeUs = nowUs;
  }

  /** The width, in microseconds, of the last pulse if it ended within `maxAgeUs` of `nowUs`; otherwise 0. */
  uint32_t widthUs(uint32_t nowUs, uint32_t maxAgeUs) const {
    if (!hasPulse || nowUs - lastEndUs > maxAgeUs) {
      return 0;
    }
    return lastWidthUs;
  }
};
