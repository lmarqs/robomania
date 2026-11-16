#pragma once

#include <Arduino.h>

#include "analog_in_pin.h"

/** An analog input on the board the sketch is built for, through analogRead(), scaled to AnalogInPin's range. */
class BoardAnalogInPin : public AnalogInPin {
 private:
  uint8_t pin;

 public:
  explicit BoardAnalogInPin(uint8_t pin);

  bool begin() override;
  uint16_t read() override;
};
