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
  periodCycles = microsecondsToClockCycles(1000000UL) / hz;
  if (periodCycles == 0) {
    return false;
  }
  actualHz = microsecondsToClockCycles(1000000UL) / periodCycles;
  pinMode(pin, OUTPUT);
  digitalWrite(pin, LOW);
  return true;
}

void BoardPwmOutPin::write(uint16_t duty) {
  if (!actualHz) {
    return;
  }
  const uint32_t highCycles = ((uint64_t)periodCycles * duty + DUTY_MAX / 2) / DUTY_MAX;
  // The generator needs time both high and low in each period, so the two ends hold the pin at a level instead.
  if (highCycles == 0 || highCycles == periodCycles) {
    stopWaveform(pin);
    digitalWrite(pin, highCycles == 0 ? LOW : HIGH);
    return;
  }
  startWaveformClockCycles(pin, highCycles, periodCycles - highCycles);
}
#endif
