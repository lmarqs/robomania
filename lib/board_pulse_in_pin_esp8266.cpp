#ifdef ESP8266
#include "board_pulse_in_pin.h"

// GPIO16 sits on the RTC, outside the GPIO peripheral that raises interrupts.
static const uint8_t LAST_INTERRUPT_PIN = 15;

bool BoardPulseInPin::begin() {
  if (pin > LAST_INTERRUPT_PIN) {
    return false;
  }
  pinMode(pin, INPUT);
  attachInterruptArg(digitalPinToInterrupt(pin), onEdge, this, CHANGE);
  return true;
}

// An interrupt handler must run from RAM: the flash cache may be busy when the edge arrives.
void IRAM_ATTR BoardPulseInPin::onEdge(void* self) {
  BoardPulseInPin* input = (BoardPulseInPin*)self;
  const uint32_t nowUs = micros();
  const bool high = digitalRead(input->pin) == HIGH;
  // A falling edge counts only after a rising one, so the first pulse after begin() is never half of one.
  if (!high && input->high) {
    input->widthUs = nowUs - input->lastEdgeUs;
    input->endUs = nowUs;
  }
  input->high = high;
  input->lastEdgeUs = nowUs;
}

uint32_t BoardPulseInPin::read(uint32_t maxAgeUs) {
  noInterrupts();
  const uint32_t width = widthUs;
  const uint32_t end = endUs;
  interrupts();
  if (end == 0 || micros() - end > maxAgeUs) {
    return 0;
  }
  return width;
}
#endif
