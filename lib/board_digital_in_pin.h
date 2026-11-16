#pragma once

#include <Arduino.h>

#include "digital_in_pin.h"

/**
 * A digital input on the board the sketch is built for. `mode` is pinMode()'s, such as INPUT_PULLUP for a button to
 * ground, which then reads on when the pin is LOW: pass LOW as `activeLevel`.
 */
class BoardDigitalInPin : public DigitalInPin {
 private:
  uint8_t pin;
  uint8_t mode;
  uint8_t activeLevel;

 public:
  explicit BoardDigitalInPin(uint8_t pin, uint8_t mode = INPUT, uint8_t activeLevel = HIGH);

  bool begin() override;
  bool read() override;
};
