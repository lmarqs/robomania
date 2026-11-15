#pragma once

#ifdef ESP32
#include <Arduino.h>

/**
 * A pulse input on an ESP32 GPIO, timed by an interrupt on each edge, so reading it never waits for a pulse. The pin
 * is pulled down, so a loose wire gives no pulses instead of noise.
 */
class EspPulseInPin {
 private:
  uint8_t pin;
  portMUX_TYPE lock = portMUX_INITIALIZER_UNLOCKED;
  bool high = false;
  uint32_t lastEdgeUs = 0;
  uint32_t widthUs = 0;
  uint32_t endUs = 0;

  static void onEdge(void* self);

 public:
  explicit EspPulseInPin(uint8_t pin);
  /** Returns false when the pin cannot run, such as when it has no interrupt. */
  bool begin();
  /** The width, in microseconds, of the last high pulse if it ended within the last `maxAgeUs`; otherwise 0. */
  uint32_t read(uint32_t maxAgeUs);
};
#endif
