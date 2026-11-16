#pragma once

#include <Arduino.h>

#include "pulse_in_pin.h"
#include "pulse_width_meter.h"

/**
 * A pulse input on the board the sketch is built for, timed by an interrupt on each edge, so reading it never waits
 * for a pulse:
 *
 * - ESP32: on any GPIO, pulled down, so a loose wire gives no pulses instead of noise.
 * - ESP8266: on any GPIO but GPIO16, which has no interrupt, or the flash chip's pins.
 * - AVR: on the first two external interrupts' pins, 2 and 3 on an Uno or Nano. micros() counts in 4 µs steps
 *   there, so a width is read to 4 µs.
 */
class BoardPulseInPin : public PulseInPin {
 private:
  uint8_t pin;
  PulseWidthMeter meter;
#ifdef ESP32
  portMUX_TYPE lock = portMUX_INITIALIZER_UNLOCKED;
#endif

  static void onEdge(void* self);
#ifdef __AVR__
  static void onSlot0();
  static void onSlot1();
#endif

 public:
  explicit BoardPulseInPin(uint8_t pin);

  bool begin() override;
  uint32_t read(uint32_t maxAgeUs) override;
};
