#ifdef __AVR__
#include "board_pulse_in_pin.h"

// attachInterrupt() takes a handler without an argument, so each interrupt gets its own, which finds its pin here.
static const uint8_t SLOT_COUNT = 2;
static BoardPulseInPin* slots[SLOT_COUNT];

void BoardPulseInPin::onSlot0() { onEdge(slots[0]); }
void BoardPulseInPin::onSlot1() { onEdge(slots[1]); }

bool BoardPulseInPin::begin() {
  const int interrupt = digitalPinToInterrupt(pin);
  if (interrupt == NOT_AN_INTERRUPT || interrupt >= SLOT_COUNT) {
    return false;
  }
  pinMode(pin, INPUT);
  slots[interrupt] = this;
  attachInterrupt(interrupt, interrupt == 0 ? onSlot0 : onSlot1, CHANGE);
  return true;
}

void BoardPulseInPin::onEdge(void* self) {
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
