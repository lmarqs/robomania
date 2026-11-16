#include "board_analog_in_pin.h"

// The resolution analogRead() returns by default: 12 bits on the ESP32 and ESP32-C3, 10 bits on the ESP8266 and AVR.
#if defined(ESP32)
static const uint32_t ADC_MAX = 4095;
#else
static const uint32_t ADC_MAX = 1023;
#endif

BoardAnalogInPin::BoardAnalogInPin(uint8_t pin) : pin(pin) {}

bool BoardAnalogInPin::begin() {
  pinMode(pin, INPUT);
  return true;
}

uint16_t BoardAnalogInPin::read() {
  // The ESP8266's SDK documents its ADC as reading up to 1024, one past its 10 bits.
  const uint32_t raw = analogRead(pin);
  return ((raw < ADC_MAX ? raw : ADC_MAX) * (uint32_t)VALUE_MAX + ADC_MAX / 2) / ADC_MAX;
}
