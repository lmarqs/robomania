#ifdef ESP32
#include "pulse_in_pin.h"

EspPulseInPin::EspPulseInPin(uint8_t pin) { this->pin = pin; }

bool EspPulseInPin::begin() {
  if (digitalPinToInterrupt(pin) == NOT_AN_INTERRUPT) {
    return false;
  }
  pinMode(pin, INPUT_PULLDOWN);
  attachInterruptArg(digitalPinToInterrupt(pin), onEdge, this, CHANGE);
  return true;
}

void ARDUINO_ISR_ATTR EspPulseInPin::onEdge(void* self) {
  EspPulseInPin* input = (EspPulseInPin*)self;
  const uint32_t nowUs = micros();
  const bool high = digitalRead(input->pin) == HIGH;
  portENTER_CRITICAL_ISR(&input->lock);
  // A falling edge counts only after a rising one, so the first pulse after begin() is never half of one.
  if (!high && input->high) {
    input->widthUs = nowUs - input->lastEdgeUs;
    input->endUs = nowUs;
  }
  input->high = high;
  input->lastEdgeUs = nowUs;
  portEXIT_CRITICAL_ISR(&input->lock);
}

uint32_t EspPulseInPin::read(uint32_t maxAgeUs) {
  portENTER_CRITICAL(&lock);
  const uint32_t width = widthUs;
  const uint32_t end = endUs;
  portEXIT_CRITICAL(&lock);
  if (end == 0 || micros() - end > maxAgeUs) {
    return 0;
  }
  return width;
}
#endif
