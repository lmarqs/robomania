#include <Arduino.h>
#include <board_pulse_in_pin.h>
#include <serial_output.h>

#if __has_include("local.h")
#include "local.h"
#endif

// The GPIO each receiver channel is wired to, channel 1 first. An Uno or Nano times pulses only on pins 2 and 3.
#ifndef RC_RECEIVER_PINS
#ifdef __AVR__
#define RC_RECEIVER_PINS 2, 3
#else
#define RC_RECEIVER_PINS 5, 17, 16, 4, 2, 15
#endif
#endif

const uint8_t PINS[] = {RC_RECEIVER_PINS};
const size_t CHANNELS = sizeof(PINS) / sizeof(PINS[0]);
// A receiver repeats its pulses every 20 ms or faster, so a channel silent for longer has lost its signal.
const uint32_t SIGNAL_TIMEOUT_US = 100000;
const uint32_t PRINT_EVERY_MS = 100;

static BoardPulseInPin* inputs[CHANNELS];
static SerialOutput output;

void setup() {
  output.begin("rc-receiver");
  for (size_t i = 0; i < CHANNELS; i++) {
    inputs[i] = new BoardPulseInPin(PINS[i]);
    if (!inputs[i]->begin()) {
      output.printf("error gpio %u has no interrupt\n", PINS[i]);
    }
    output.printf("channel %u gpio %u\n", i + 1, PINS[i]);
  }
}

void loop() {
  for (size_t i = 0; i < CHANNELS; i++) {
    const uint32_t widthUs = inputs[i]->read(SIGNAL_TIMEOUT_US);
    if (widthUs == 0) {
      output.printf("ch%u - ", i + 1);
    } else {
      output.printf("ch%u %lu ", i + 1, (unsigned long)widthUs);
    }
  }
  output.println();
  delay(PRINT_EVERY_MS);
}
