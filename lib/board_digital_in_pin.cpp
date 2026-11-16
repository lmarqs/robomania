#include "board_digital_in_pin.h"

BoardDigitalInPin::BoardDigitalInPin(uint8_t pin, uint8_t mode, uint8_t activeLevel)
    : pin(pin), mode(mode), activeLevel(activeLevel) {}

bool BoardDigitalInPin::begin() {
  pinMode(pin, mode);
  return true;
}

bool BoardDigitalInPin::read() { return digitalRead(pin) == activeLevel; }
