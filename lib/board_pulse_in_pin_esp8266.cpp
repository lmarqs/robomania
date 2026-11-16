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
  input->timer.recordEdge(digitalRead(input->pin) == HIGH, micros());
}

uint32_t BoardPulseInPin::read(uint32_t maxAgeUs) {
  noInterrupts();
  const PulseTimer snapshot = timer;
  interrupts();
  return snapshot.widthUs(micros(), maxAgeUs);
}
#endif
