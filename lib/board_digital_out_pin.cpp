#include "board_digital_out_pin.h"

BoardDigitalOutPin::BoardDigitalOutPin(uint8_t pin, uint8_t activeLevel) : pin(pin), activeLevel(activeLevel) {}

bool BoardDigitalOutPin::begin() {
  pinMode(pin, OUTPUT);
  write(false);
  return true;
}

void BoardDigitalOutPin::write(bool on) {
  const uint8_t inactiveLevel = activeLevel == HIGH ? LOW : HIGH;
  digitalWrite(pin, on ? activeLevel : inactiveLevel);
}
