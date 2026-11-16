#ifdef ESP32
#include "board_pulse_in_pin.h"

bool BoardPulseInPin::begin() {
  if (digitalPinToInterrupt(pin) == NOT_AN_INTERRUPT) {
    return false;
  }
  pinMode(pin, INPUT_PULLDOWN);
  attachInterruptArg(digitalPinToInterrupt(pin), onEdge, this, CHANGE);
  return true;
}

void ARDUINO_ISR_ATTR BoardPulseInPin::onEdge(void* self) {
  BoardPulseInPin* input = (BoardPulseInPin*)self;
  const uint32_t nowUs = micros();
  const bool high = digitalRead(input->pin) == HIGH;
  portENTER_CRITICAL_ISR(&input->lock);
  input->meter.recordEdge(high, nowUs);
  portEXIT_CRITICAL_ISR(&input->lock);
}

uint32_t BoardPulseInPin::read(uint32_t maxAgeUs) {
  portENTER_CRITICAL(&lock);
  const PulseWidthMeter snapshot = meter;
  portEXIT_CRITICAL(&lock);
  return snapshot.widthUs(micros(), maxAgeUs);
}
#endif
