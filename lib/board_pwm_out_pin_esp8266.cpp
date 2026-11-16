#ifdef ESP8266
#include <core_esp8266_waveform.h>

#include "board_pwm_out_pin.h"

// The rate analogWrite() starts at in core 3.
static const uint32_t ANALOG_WRITE_HZ = 1000;
// GPIO16 is the last pin the waveform generator drives; it also refuses the pins isFlashInterfacePin() names.
static const uint8_t LAST_PIN = 16;

bool BoardPwmOutPin::begin() {
  if (actualHz) {
    return true;
  }
  if (pin > LAST_PIN || isFlashInterfacePin(pin)) {
    return false;
  }
  const uint32_t hz = requestedHz ? requestedHz : ANALOG_WRITE_HZ;
  steps = microsecondsToClockCycles(1000000UL) / hz;
  if (steps == 0) {
    return false;
  }
  actualHz = microsecondsToClockCycles(1000000UL) / steps;
  pinMode(pin, OUTPUT);
  digitalWrite(pin, LOW);
  return true;
}

// Each step is one CPU clock cycle.
void BoardPwmOutPin::writeSteps(uint32_t highSteps) {
  // The generator needs time both high and low in each period, so the two ends hold the pin at a level instead.
  if (highSteps == 0 || highSteps == steps) {
    stopWaveform(pin);
    digitalWrite(pin, highSteps == 0 ? LOW : HIGH);
    return;
  }
  startWaveformClockCycles(pin, highSteps, steps - highSteps);
}
#endif
