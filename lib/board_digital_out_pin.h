#pragma once

#include <Arduino.h>

#include "digital_out_pin.h"

/**
 * A digital output on the board the sketch is built for. Its active level is the one that turns its load on: HIGH,
 * unless the circuit wires the load from the supply to the pin, as the ESP-01S does its LED.
 */
class BoardDigitalOutPin : public DigitalOutPin {
 private:
  uint8_t pin;
  uint8_t activeLevel;

 public:
  explicit BoardDigitalOutPin(uint8_t pin, uint8_t activeLevel = HIGH);

  bool begin() override;
  void write(bool on) override;
};
